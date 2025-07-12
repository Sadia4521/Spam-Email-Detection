#include<iostream>
#include<string>
#include<iomanip>
#include <unordered_set>
using namespace std;
unordered_set<string> stopWords = {
    "a", "an", "the", "in", "on", "at", "to", "for", "by", "with",
    "and", "or", "but", "is", "are", "was", "were", "this", "that",
    "of", "it", "as", "be", "from", "has", "have", "had", "will",
    "would", "can", "could", "should", "do", "does", "did", "not"
};
// Email structure
struct Email {
    string address;
    string header;
    string body;
    int id;                        // Email ID
    string* keywords;              // Array of keywords associated with the email
    int keywordCount;              // Number of keywords in the email
};
// Define the structure for a linked list node containing Email
struct EmailNode {
    Email email;
    EmailNode* next;
};
// Function to add an email to the linked list
void addEmail(EmailNode*& head, const Email& email) {
    EmailNode* newNode = new EmailNode;
    newNode->email = email;
    newNode->next = nullptr;
    if (head == nullptr) {
        head = newNode;  // Set head if the list is empty
    }
    else {
        EmailNode* current = head;
        while (current->next != nullptr) {
            current = current->next;  // Traverse to the end of the list
        }
        current->next = newNode;  // Add the new email at the end
    }
}
// Node for hashmap
struct Node {
    string keyword;
    int value;
    Node* next;
    Node(string k, int v) : keyword(k), value(v), next(nullptr) {}
};
// Hashmap class
class Hashmap {
    int tablesize;
    Node** table;
    // Hash map function for allocating space for keyword
    int hashFunction(const string& key) {
        int hash = 0;
        for (char ch : key) {
            hash = (hash * 31 + ch) % tablesize;
        }
        return hash;
    }
public:
    Hashmap(int size) {
        tablesize = size;
        table = new Node * [tablesize];
        for (int i = 0; i < tablesize; i++) {
            table[i] = nullptr;
        }
    }
    ~Hashmap() {
        for (int i = 0; i < tablesize; i++) {
            Node* current = table[i];
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }
    // Method to get value of keyword in hash table
    int get(const string& key) {
        int hashIndex = hashFunction(key);
        Node* current = table[hashIndex];
        // traversing
        while (current) {
            if (current->keyword == key) {
                return current->value; // if spam keyword found return its value else return 0
            }
            current = current->next;
        }
        return 0;
    }
    void insert(const string& key, int value) {
        int hashIndex = hashFunction(key);
        Node* current = table[hashIndex];
        while (current) {
            if (current->keyword == key) {
                current->value = value; // Update value of key if already exists
                return;
            }
            current = current->next;
        }
        // Add new node at the start of the list
        Node* newNode = new Node(key, value);
        newNode->next = table[hashIndex];
        table[hashIndex] = newNode;
    }
};
// Class to handle email operations, including enqueueing emails and spam detection
class Email_Sender {
private:
    int front;
    int rear;
    Email* emailQueue;
    int maxQueue;
    Hashmap spamData;
public:
    Email_Sender(int size) : spamData(10) {
        emailQueue = new Email[size];
        maxQueue = size;
        front = -1;
        rear = -1;
        initializeSpamData();
    }
    ~Email_Sender() {
        delete[] emailQueue;
    }
    void initializeSpamData() {
        string keywords[] = {
            "congratulations", "won", "prize", "discount", "big deal", "free", "limited time", "urgent",
            "offer", "deal", "winner", "claim", "reward", "cash", "loan", "gift", "save", "bonus",
            "promo", "click", "earn", "exclusive", "special", "luxury", "cheap", "buy", "promotion",
            "investment", "subscribe"
        };
        // Assign values to keywords stored in the spam dataset
        for (const string& keyword : keywords) {
            spamData.insert(keyword, 1); // Assigning 1 value to all keywords
        }
    }
    bool isFull() const {
        return rear == maxQueue - 1;
    }
    bool isEmpty() const {
        return front == -1;
    }
    void Enqueue(Email data) {
        if (isFull()) {
            cout << "Queue is full! Cannot add more emails." << endl;
        }
        else {
            if (front == -1) {
                front = 0;
            }
            rear++;
            emailQueue[rear] = data;
        }
    }
    // to calculate the spam score
    int calculateSpamScore(const Email& email) {
        int score = 0;
        string content = email.header + " " + email.body;
        string word;
        for (char ch : content) {
            if (isalnum(ch)) {
                word += tolower(ch);
            }
            else if (!word.empty()) {
                if (stopWords.find(word) == stopWords.end()) { // Skip if the word is in stopWords
                    score += spamData.get(word);
                }
                word.clear();
            }
        }
        if (!word.empty() && stopWords.find(word) == stopWords.end()) {
            score += spamData.get(word);
        }
        return score;
    }

    void displaySpamScores() {
        if (isEmpty()) {
            cout << "No emails to process!" << endl;
        }
        else {
            for (int i = front; i <= rear; i++) {
                int spamScore = calculateSpamScore(emailQueue[i]);
                cout << "*****************----------++------------****************" << endl;
                cout << "Sender: " << emailQueue[i].address << endl;
                cout << "Header: " << emailQueue[i].header << endl;
                cout << "Body: " << emailQueue[i].body << endl;
                cout << "-----------------------------------" << endl;
                cout << "|" << setw(10) << "Spam Score: " << spamScore << endl;
                cout << "__________________________________" << endl;
                // Determine if the email is spam or legitimate
                if (spamScore > 2) {
                    cout << "Result: This email is SPAM." << endl;
                }
                else {
                    cout << "Result: This email is LEGITIMATE." << endl;
                }
            }
        }
    }
    void displayResults() {
        // Define the complete spam keywords list
        unordered_set<string> spamKeywords = {
            "congratulations", "won", "prize", "discount", "big deal", "free", "limited time", "urgent",
            "offer", "deal", "winner", "claim", "reward", "cash", "loan", "gift", "save", "bonus",
            "promo", "click", "earn", "exclusive", "special", "luxury", "cheap", "buy", "promotion",
            "investment", "subscribe"
        };
        unordered_set<int> spamFolder, inbox;
        // Update spam folders and inbox based on spam score
        for (int i = front; i <= rear; i++) {
            int spamScore = calculateSpamScore(emailQueue[i]);
            if (spamScore > 2) {
                spamFolder.insert(i);
            }
            else {
                inbox.insert(i);
            }
        }
        // Display results in desired format
        cout << "Updated Spam Keywords: ";
        for (const string& keyword : spamKeywords) {
            cout << keyword << " ";
        }
        cout << endl;
        cout << "Spam Folder: ";
        for (int i : spamFolder) {
            cout << i << " ";
        }
        cout << endl;
        cout << "Inbox: ";
        for (int i : inbox) {
            cout << i << " ";
        }
        cout << endl;
    }
};
int main() {
    Email_Sender sender(8);
    // Legitimate emails
    Email email1{ "qaswarsarfraz1@gmail.com", "Meeting the Schedule", "Let's schedule a meeting for tomorrow." };
    Email email2{ "qaswarsarfraz2@gmail.com", "Project the Update", "The project update will be shared soon." };
    // Spam emails
    Email email3{ "spamemail1@gmail.com", "Congratulations!", "You won a prize! Claim your discount now." };
    Email email4{ "spamemail2@gmail.com", "Limited Time Offer", "Get a free gift with your purchase." };
    Email email5{ "spamemail3@gmail.com", "Urgent", "Act now to take advantage of this big deal." };
    sender.Enqueue(email1);
    sender.Enqueue(email2);
    sender.Enqueue(email3);
    sender.Enqueue(email4);
    sender.Enqueue(email5);
    sender.displaySpamScores();
    sender.displayResults();
    return 0;
}
