#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

// ========================
//       USER CLASS
// ========================
class User {
private:
    string username;
    string password;
    string phoneNumber;
    string status;
    string lastSeen;

public:
     User() {
        username = "";
        password = "";
        phoneNumber = "";
        status = "";
        lastSeen = "";

    }

    User(string uname, string pwd, string phone) {
      username = uname;
      password = pwd;
      phoneNumber = phone;
    }

    string getUsername() const {

       return username;
    }

    string getPhoneNumber() const {

        return phoneNumber;
    }

    string getStatus() const {

        return status;
    }

    string getLastSeen() const {

        return lastSeen;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void setPhoneNumber(string phone) {
        phoneNumber = phone;
    }

    void updateLastSeen() {
        // TODO: Implement last seen update
    }

    bool checkPassword(string pwd) const {
        // TODO: Implement password check
        return false;
    }

    void changePassword(string newPwd) {
        // TODO: Implement password change
    }
};

// ========================
//      MESSAGE CLASS
// ========================
class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;
    Message* replyTo;

public:
    Message() {
        // TODO: Implement default constructor
    }

    Message(string sndr, string cntnt) {
        // TODO: Implement parameterized constructor
    }

    string getContent() const {
        // TODO: Implement getter
        return "";
    }

    string getSender() const {
        // TODO: Implement getter
        return "";
    }

    string getTimestamp() const {
        // TODO: Implement getter
        return "";
    }

    string getStatus() const {
        // TODO: Implement getter
        return "";
    }

    Message* getReplyTo() const {
        // TODO: Implement getter
        return nullptr;
    }

    void setStatus(string newStatus) {
        // TODO: Implement setter
    }

    void setReplyTo(Message* msg) {
        // TODO: Implement setter
    }

    void updateTimestamp() {
        // TODO: Implement timestamp update
    }

    void display() const {
        // TODO: Implement message display
    }

    void addEmoji(string emojiCode) {
        // TODO: Implement emoji support
    }
};

// ========================
//       CHAT CLASS (BASE)
// ========================
class Chat {
protected:
    vector<string> participants;
    vector<Message> messages;
    string chatName;

public:
    Chat() {
        // TODO: Implement default constructor
    }

    Chat(vector<string> users, string name) {
        participants = users;
        chatName = name;
    }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    // QA FIX: Changed 'int index' to 'size_t index' to prevent compiler warnings
    bool deleteMessage(size_t index, const string& username) {
        if (index >= 0 && index < messages.size()) {
            if (messages[index].getSender() == username) {
                
                // --- QA TICKET 12: DANGLING POINTER FIX START ---
                Message* msgToDelete = &messages[index];
                for (size_t i = 0; i < messages.size(); i++) {
                    if (messages[i].getReplyTo() == msgToDelete) {
                        messages[i].setReplyTo(nullptr);
                    }
                }
                // --- QA TICKET 12: DANGLING POINTER FIX END ---

                messages.erase(messages.begin() + index);
                return true;
            }
        }
        return false;
    }

    virtual void displayChat() const {
        // QA FIX: Changed 'int i' to 'size_t i' to prevent compiler warnings
        for (size_t i = 0; i < messages.size(); i++) {
            messages[i].display();
        }
    }

    vector<Message> searchMessages(string keyword) const {
        vector<Message> matches;
        // QA FIX: Changed 'int i' to 'size_t i' to prevent compiler warnings
        for (size_t i = 0; i < messages.size(); i++) {
            if (messages[i].getContent().find(keyword) != string::npos) {
                matches.push_back(messages[i]);
            }
        }
        return matches;
    }

    void exportToFile(const string& filename) const {
        // TODO: Implement export to file
    }
};

// ========================
//     PRIVATE CHAT CLASS
// ========================
class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(string u1, string u2) : Chat(vector<string>{u1, u2}, "Chat between " + u1 + " and " + u2) {
        user1 = u1;
        user2 = u2;
    }

    void displayChat() const override {
        cout << "\n=== Private Chat ===\n";
        cout << user1 << " and " << user2 << endl;

        if (messages.empty()) {
            cout << "No messages yet.\n";
            return;
        }

        // QA FIX: Changed 'int i' to 'size_t i' to prevent compiler warnings
        for (size_t i = 0; i < messages.size(); i++) {
            messages[i].display();
        }
    }

    void showTypingIndicator(const string& username) const {
        if (username == user1 || username == user2) {
            cout << username << " is typing...\n";
        }
        else {
            cout << "User is not part of this private chat.\n";
        }
    }
};

// ========================
//      GROUP CHAT CLASS
// ========================
class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;

public:
    GroupChat(vector<string> users, string name, string creator) : Chat(users, name) {
        description = "";

        if (!isParticipant(creator)) {
            participants.push_back(creator);
        }

        admins.push_back(creator);
    }

    void addAdmin(string newAdmin) {
        if (!isParticipant(newAdmin)) {
            cout << "Cannot add admin. User is not a participant.\n";
            return;
        }

        if (isAdmin(newAdmin)) {
            cout << newAdmin << " is already an admin.\n";
            return;
        }

        admins.push_back(newAdmin);
        cout << newAdmin << " is now an admin.\n";
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        if (!isAdmin(admin)) {
            cout << "Only admins can remove participants.\n";
            return false;
        }

        if (!isParticipant(userToRemove)) {
            cout << "User is not in this group.\n";
            return false;
        }

        participants.erase(remove(participants.begin(), participants.end(), userToRemove), participants.end());
        admins.erase(remove(admins.begin(), admins.end(), userToRemove), admins.end());

        cout << userToRemove << " was removed from the group.\n";
        return true;
    }

    bool isAdmin(string username) const {
        return find(admins.begin(), admins.end(), username) != admins.end();
    }

    bool isParticipant(string username) const {
        return find(participants.begin(), participants.end(), username) != participants.end();
    }

    void setDescription(string desc) {
        description = desc;
    }

    void displayChat() const override {
        cout << "\n=== Group Chat: " << chatName << " ===\n";

        if (!description.empty()) {
            cout << "Description: " << description << endl;
        }

        cout << "Participants: ";
        for (size_t i = 0; i < participants.size(); i++) {
            cout << participants[i] << " ";
        }

        cout << "\nAdmins: ";
        for (size_t i = 0; i < admins.size(); i++) {
            cout << admins[i] << " ";
        }

        cout << "\nMessages:\n";

        if (messages.empty()) {
            cout << "No messages yet.\n";
        }
        else {
            for (size_t i = 0; i < messages.size(); i++) {
                messages[i].display();
            }
        }
    }

    void sendJoinRequest(const string& username) {
        cout << username << " requested to join group: " << chatName << endl;
    }
};

// ========================
//    WHATSAPP APP CLASS
// ========================
class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;

  int findUserIndex(string username) const {
        for(int i=0;i<users.size();i++)
        {
            if (users[i].getUsername() == username)

            {
                return i;
            }
        }
        return -1;
    }
    
    bool isLoggedIn() const {          // ana
        if (currentUserIndex >= 0 && currentUserIndex < users.size()) {
            return true;
        }
        return false;
    }
    
    string getCurrentUsername() const {     // ana
        if (isLoggedIn()) {
            return users[currentUserIndex].getUsername();
        }
        return "";
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    ~WhatsApp()
    {
        for ( Chat*chat: chats)
        {
            delete chat;
        }
    }
    void signUp() {
        string username,password,phoneNumber;
        while (true) {
            cout << "Enter username: ";
            cin >> username;
            if (findUserIndex(username) != -1) {
                cout << "Username already exists. Please choose another.\n";
            } else {
                break;
            }
        }
        while (true) {
            cout << "Enter password: ";
            cin >> password;
            if (password.length() < 6) {
                cout << "Password must be at least 6 characters.\n";
            } else {
                break;
            }
        }
        while (true) {
            cout << "Enter phone number: ";
            cin >> phoneNumber;
            if (phoneNumber.length() != 11) {
                cout << "Phone number must be 11 digits.\n";
            } else {
                break;
            }
        }
       users.push_back(User(username, password, phoneNumber));
       cout << "Sign up successful!.\n";  
       cout << "Now you may login...\n";
    }
   
    
    void login() {             
       string username;
       string password;
       cout << "Enter username: ";
       cin >> username;
       int index = findUserIndex(username);
       if(index == -1) {
           cout << "User not found.\n";
           return;
       }
       cout << "Enter password: ";
       cin >> password;
       if (!users[index].checkPassword(password)) {
           cout << "Incorrect password.\n";
           return;
       }
       currentUserIndex = index;
       cout << "login succesfully...\n";
    }

    void startPrivateChat() {
        // TODO: Implement private chat creation
    }

    void createGroup() {
        // TODO: Implement group creation
    }

    void viewChats() const {
        // TODO: Implement chat viewing
    }
 void logout() {    
        currentUserIndex = -1;
        cout << "Logged out successfully.\n";
    }

    void run() {
        while (true) {
            if (!isLoggedIn()) {
                cout << "\n1. Login\n2. Sign Up\n3. Exit\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) break;
            }
            else {
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Logout\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) logout();
            }
        }
    }
};

// ========================
//          MAIN
// ========================
int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}