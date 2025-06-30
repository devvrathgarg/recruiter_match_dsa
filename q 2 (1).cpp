#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<cstring>
#include<cctype>
using namespace std;
struct Recruiter {
    string name;
    string location;
    int experienceYears;
    string industry;
    int lb;
    int ub;
    vector<pair<string,int> >reqSkills;

    // Recruiter(string n, string loc, int exp,string ind,int lob,int uob,vector<pair<string,int> >rs)
    //     : name(n),industrie(ind) ,location(loc),reqSkills(rs),lb(lob),ub(uob) ,experienceYears(exp) {}
};

class TrieNode {
public:
    std::unordered_map<char, std::shared_ptr<TrieNode> > children;
    std::unordered_map<int, unordered_map<string,std::vector<Recruiter> > > experienceMap;

    // TrieNode() = default;
};


class RecruiterTrie {
private:
    std::shared_ptr<TrieNode> root;

public:
    RecruiterTrie() : root(std::make_shared<TrieNode>()) {}

    // Insert a recruiter into the Trie based on industry and experience
    void insertRecruiter(const std::string &industry, const Recruiter &recruiter,string location) {
        std::shared_ptr<TrieNode> node = root;

        // Traverse or create nodes in the Trie for each character in the industry name
        for (char ch : industry) {
            if (!node->children.count(ch)) {
                node->children[ch] = std::make_shared<TrieNode>();
            }
            node = node->children[ch];
        }

        // Insert the recruiter into the experience map at the terminal node for this industry
        node->experienceMap[recruiter.experienceYears][location].push_back(recruiter);
    }

    // Find recruiters by industry prefix and minimum experience
    std::vector<Recruiter> findRecruitersByPrefix(const std::string &prefix, int minExperience,string location="") {
        std::shared_ptr<TrieNode> node = root;
        std::vector<Recruiter> matchingRecruiters;
        // Traverse the Trie based on the prefix
        for (char ch : prefix) {
            if (!node->children.count(ch)) {  // Prefix not found
                return matchingRecruiters;  // Return empty vector if prefix doesn't match
            }
            node = node->children[ch];
        }

        // Perform a depth-first search (DFS) from the current node to gather matching recruiters
        gatherRecruiters(node, minExperience, matchingRecruiters,location);
        return matchingRecruiters;
    }

private:
    // Helper function to gather recruiters from a node and its descendants
    void gatherRecruiters(const std::shared_ptr<TrieNode> &node, int minExperience, std::vector<Recruiter> &recruiters,string location) {
        // Collect recruiters with experience greater than or equal to minExperience
        for (auto &[exp, recList] : node->experienceMap) {
            if (exp <= minExperience) {
                if(location=="no"){
                    for(const auto &[loc,recruiter_name]:recList){
                        recruiters.insert(recruiters.end(), recruiter_name.begin(), recruiter_name.end());
                    }
                }
                else{
                    recruiters.insert(recruiters.end(), recList[location].begin(), recList[location].end());
                }
            }
        }

        // Recur for each child node
        for (const auto &[ch, childNode] : node->children) {
            gatherRecruiters(childNode, minExperience, recruiters,location);
        }
    }
};


class MaxHeap {
private:
    vector<pair<double, Recruiter> > heap;  // The underlying storage for the heap

    // Helper function to maintain the max heap property by moving an element up
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;  // Calculate the parent index
            if (heap[index].first > heap[parent].first) {
                swap(heap[index], heap[parent]);  // Swap with parent if the current element is larger
                index = parent;  // Move up to the parent index
            } else {
                break;  // The max-heap property is satisfied
            }
        }
    }

    // Helper function to maintain the max heap property by moving an element down
    void heapifyDown(int index) {
        int size = heap.size();
        while (index < size) {
            int leftChild = 2 * index + 1;  // Calculate left child index
            int rightChild = 2 * index + 2; // Calculate right child index
            int largest = index;  // Assume the largest is the current index

            // Check if the left child exists and is greater than the current largest
            if (leftChild < size && heap[leftChild].first > heap[largest].first) {
                largest = leftChild;
            }
            // Check if the right child exists and is greater than the current largest
            if (rightChild < size && heap[rightChild].first > heap[largest].first) {
                largest = rightChild;
            }
            // If the largest is not the current index, swap and continue down the heap
            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;  // Move down to the largest index
            } else {
                break;  // The max-heap property is satisfied
            }
        }
    }

public:
    // Add a recruiter to the heap
    void push(const pair<double, Recruiter>& element) {
        heap.push_back(element);  // Add the element to the end
        heapifyUp(heap.size() - 1);  // Maintain the heap property
    }

    // Remove and return the maximum element (the root of the heap)
    pair<double, Recruiter> pop() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");  // Handle empty heap case
        }
        pair<double, Recruiter> maxElement = heap[0];  // Store the max element
        heap[0] = heap.back();  // Move the last element to the root
        heap.pop_back();  // Remove the last element
        heapifyDown(0);  // Maintain the heap property
        return maxElement;  // Return the maximum element
    }

    // Peek at the maximum element without removing it
    pair<double, Recruiter> peek() const {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        }
        return heap[0];  // Return the root element
    }

    // Check if the heap is empty
    bool empty() const {
        return heap.empty();
    }

    // Get the size of the heap
    size_t size() const {
        return heap.size();
    }
};
string removeSpacesAndToLower(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != ' ') {
            result += std::tolower(c);  // Convert to lowercase and add to result
        }
    }
    return result;
}
int main() {
    // Create a Recruiter Trie
    RecruiterTrie trie;
    std::ifstream file("a.txt");  // Open the file
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    std::string line;
    std::vector<Recruiter> recruiters;

    while (std::getline(file, line)) {
        Recruiter recruiter;
        
        // Read Recruiter Name
        recruiter.name = removeSpacesAndToLower(line.substr(line.find(":") + 2));
        
        // Read Location
        std::getline(file, line);
        recruiter.location = removeSpacesAndToLower(line.substr(line.find(":") + 2));
        
        // Read Experience Years
        std::getline(file, line);
        recruiter.experienceYears = std::stoi(line.substr(line.find(":") + 2));
        
        // Read Industry
        std::getline(file, line);
        recruiter.industry = removeSpacesAndToLower(line.substr(line.find(":") + 2));
        
        // Read Lower Bound Salary
        std::getline(file, line);
        recruiter.lb = std::stoi(line.substr(line.find(":") + 2));
        
        // Read Upper Bound Salary
        std::getline(file, line);
        recruiter.ub = std::stoi(line.substr(line.find(":") + 2));
        
        // Read Required Skills
        std::getline(file, line);
        std::string skillsStr = line.substr(line.find(":") + 2);
        std::istringstream skillStream(removeSpacesAndToLower(skillsStr));
        std::string skill;
        while (std::getline(skillStream, skill, ',')) {
            std::string skillName = skill.substr(0, skill.find(":"));
            int skillLevel = std::stoi(skill.substr(skill.find(":") + 1));
            recruiter.reqSkills.emplace_back(skillName, skillLevel);
        }

        recruiters.push_back(recruiter);
        
        // Read the empty line between recruiters
        std::getline(file, line);
    }

    file.close();  // Close the file



//Candidate Details


    string desiredIndustry;
    int minExperience = 5;
    string location="";
    cout<<"DesiredIndustry : ";
    cin>>desiredIndustry;
    cout<<endl;
    cout<<"Experience(in Year) : ";
    cin>>minExperience;
    cout<<endl;
    cout<<"Prefered Location (if not then write 'no' :)";
    cin>>location;
    cout<<endl;

    unordered_map<string,int>candidate_skills;
    cout<<"To Find Suitable Recruiter's , we need to know your skills and for each skill give the level of your skill from (1-10)"<<endl;
    string sk;
    cout<<"When you are done then write 'q' for exit the loop."<<endl;
    cout<<"skill : ";
    cin>>sk;
    while(sk!="q"){
        cout<<endl;
        int lvl;
        cout<<"level : ";
        cin>>lvl;
        candidate_skills[removeSpacesAndToLower(sk)]=lvl;
        cout<<endl;
        cout<<"skill : ";
        cin>>sk;
    }
    //Inserting Recruiter's data in Trie

    for(auto recruiter:recruiters){
        trie.insertRecruiter(recruiter.industry,recruiter,recruiter.location);
        // cout<<recruiter.industry<<" "<<recruiter.name<<endl;
    }

    // Find recruiters who match the job seeker's criteria

    vector<Recruiter> suitableRecruiters = trie.findRecruitersByPrefix(removeSpacesAndToLower(desiredIndustry), minExperience,removeSpacesAndToLower(location));
    //Just Priotizing the Recuiters on the bases of expected salary.
MaxHeap pq;
for (auto &recruiter : suitableRecruiters) {
    int points = 0;
    bool fit = true;
    int total_points = 0;

    for (auto &skill : recruiter.reqSkills) {
        if (!candidate_skills.count(skill.first)) {
            fit = false;
            break;
        } else {
            if (candidate_skills[skill.first] >= skill.second) {
                points += candidate_skills[skill.first];
                total_points += skill.second;
            } else {
                fit = false;
                break;
            }
        }
    }

    // Ensure total_points is not zero to avoid division by zero
    if (fit) {
        int match_percentage = (points * 100) / total_points;
        double expected_salary = recruiter.lb + ((recruiter.ub - recruiter.lb) * (match_percentage - 100)) / 900.0;

        pq.push(make_pair(expected_salary, recruiter));
    }
}

// Output the prioritized recruiters with expected salary
    if (pq.empty()) {
        cout << "No recruiters found that match the criteria." << endl;
    } else {
        while (!pq.empty()) {
        auto it = pq.pop();
        cout << "Expected Salary (in lpa): " << it.first << " for " << it.second.name
             << " in " << it.second.location << " with " << it.second.experienceYears
             << " years of experience." << endl;
    }
}

    return 0;
}

