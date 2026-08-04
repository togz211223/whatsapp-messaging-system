#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
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
        updateLastSeen();
    }

    User(string uname, string pwd, string phone) {
        username = uname;
        password = pwd;
        phoneNumber = phone;
        status = "Online";
        updateLastSeen();
    }

    string getUsername() const { return username; }
    string getPhoneNumber() const { return phoneNumber; }
    string getStatus() const { return status; }
    string getLastSeen() const { return lastSeen; }

    void setStatus(string newStatus) { status = newStatus; }
    void setPhoneNumber(string phone) { phoneNumber = phone; }

    void updateLastSeen() {
        time_t now = time(0);
        lastSeen = ctime(&now);
        lastSeen.pop_back();
    }

    bool checkPassword(string pwd) const {
        return pwd == password;
    }

    void changePassword(string newPwd) {
        password = newPwd;
        updateLastSeen();
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
        sender = "";
        content = "";
        timestamp = "";
        status = "";
        replyTo = nullptr;
    }

    Message(string sndr, string cntnt) {
        sender = sndr;
        content = cntnt;
        status = "sent";
        updateTimestamp();
        replyTo = nullptr;
    }

    string getContent() const { return content; }
    string getSender() const { return sender; }
    string getTimestamp() const { return timestamp; }
    string getStatus() const { return status; }
    Message* getReplyTo() const { return replyTo; }

    void setStatus(string newStatus) { status = newStatus; }
    void setReplyTo(Message* msg) { replyTo = msg; }

    void updateTimestamp() {
        time_t now = time(0);
        timestamp = ctime(&now);
        timestamp.pop_back();
    }

    void display() const {
        if (replyTo != nullptr) {
            cout << "Replying to " << replyTo->getSender() << endl;
            string longMess = replyTo->getContent();
            if (longMess.length() > 50) {
                cout << longMess.substr(0, 50) << "..." << endl;
            }
            else {
                cout << replyTo->getContent() << endl;
            }
        }
        cout << "from:" << sender << endl;
        cout << content << endl;
        cout << status << endl;
        cout << timestamp << endl;
    }

    void addEmoji(string emojiCode) {
        string emoji;
        size_t explore = 0;
        if (emojiCode == ":)") { emoji = "\U0001F60A"; }
        else if (emojiCode == ":(") { emoji = "\U0001F61F"; }
        else if (emojiCode == ":D") { emoji = "\U0001F603"; }
        else if (emojiCode == "<3") { emoji = "\u2764"; }
        else if (emojiCode == ":thumbsup:") { emoji = "\U0001F44D"; }
        else { return; }

        while ((explore = content.find(emojiCode, explore)) != string::npos) {
            content.replace(explore, emojiCode.length(), emoji);
            explore += emoji.length();
        }
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
    Chat() {}

    Chat(vector<string> users, string name) {
        participants = users;
        chatName = name;
    }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    bool deleteMessage(size_t index, const string& username) {
        if (index >= 0 && index < messages.size()) {
            if (messages[index].getSender() == username) {
                Message* msgToDelete = &messages[index];
                for (size_t i = 0; i < messages.size(); i++) {
                    if (messages[i].getReplyTo() == msgToDelete) {
                        messages[i].setReplyTo(nullptr);
                    }
                }
                messages.erase(messages.begin() + index);
                return true;
            }
        }
        return false;
    }

    virtual void displayChat() const {
        for (size_t i = 0; i < messages.size(); i++) {
            messages[i].display();
        }
    }

    vector<Message> searchMessages(string keyword) const {
        vector<Message> matches;
        for (size_t i = 0; i < messages.size(); i++) {
            if (messages[i].getContent().find(keyword) != string::npos) {
                matches.push_back(messages[i]);
            }
        }
        return matches;
    }

    void exportToFile(const string& filename) const {}

    vector<string> getParticipants() const { return participants; }
    string getChatName() const { return chatName; }
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
        for (size_t i = 0; i < participants.size(); i++) {
            if (participants[i] == userToRemove) {
                participants.erase(participants.begin() + i);
                break;
            }
        }
        for (size_t i = 0; i < admins.size(); i++) {
            if (admins[i] == userToRemove) {
                admins.erase(admins.begin() + i);
                break;
            }
        }
        cout << userToRemove << " was removed from the group.\n";
        return true;
    }

    bool isAdmin(string username) const {
        for (size_t i = 0; i < admins.size(); i++) {
            if (admins[i] == username) { return true; }
        }
        return false;
    }

    bool isParticipant(string username) const {
        for (size_t i = 0; i < participants.size(); i++) {
            if (participants[i] == username) { return true; }
        }
        return false;
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
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].getUsername() == username) {
                return i;
            }
        }
        return -1;
    }

    bool isLoggedIn() const {
        if (currentUserIndex >= 0 && (size_t)currentUserIndex < users.size()) {
            return true;
        }
        return false;
    }

    string getCurrentUsername() const {
        if (isLoggedIn()) {
            return users[currentUserIndex].getUsername();
        }
        return "";
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    ~WhatsApp() {
        for (Chat* chat : chats) {
            delete chat;
        }
    }

    void signUp() {
        string username, password, phoneNumber;
        while (true) {
            cout << "Enter username: ";
            cin >> username;
            if (findUserIndex(username) != -1) {
                cout << "Username already exists. Please choose another.\n";
            }
            else {
                break;
            }
        }
        while (true) {
            cout << "Enter password: ";
            cin >> password;
            if (password.length() < 6) {
                cout << "Password must be at least 6 characters.\n";
            }
            else {
                break;
            }
        }
        while (true) {
            cout << "Enter phone number: ";
            cin >> phoneNumber;
            if (phoneNumber.length() != 11) {
                cout << "Phone number must be 11 digits.\n";
            }
            else {
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
        if (index == -1) {
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
        cout << "Login successfully...\n";
    }

    void startPrivateChat() {
        if (!isLoggedIn()) {
            cout << "Please log in first!\n";
            return;
        }

        users[currentUserIndex].updateLastSeen();

        string targetUser;
        cout << "\n--- Start Private Chat ---\n";
        cout << "Enter target username: ";
        cin >> targetUser;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (targetUser == getCurrentUsername()) {
            cout << "Operation rejected.\n";
            return;
        }

        if (findUserIndex(targetUser) == -1) {
            cout << "Error: User does not exist!\n";
            return;
        }

        Chat* newChat = new PrivateChat(getCurrentUsername(), targetUser);
        chats.push_back(newChat);

        cout << "Private chat created successfully!\n";
    }

    void createGroup() {
        if (!isLoggedIn()) {
            cout << "Please log in first!\n";
            return;
        }

        users[currentUserIndex].updateLastSeen();

        string groupName, description, secondUser;
        cout << "\n--- Create Group Chat ---\n";
        cout << "Enter Group Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, groupName);

        cout << "Enter Group Description: ";
        getline(cin, description);

        cout << "Enter a second valid username to add (minimum 2 participants required): ";
        getline(cin, secondUser);

        if (groupName.empty() || secondUser == getCurrentUsername() || findUserIndex(secondUser) == -1) {
            cout << "Operation rejected / returns false\n";
            return;
        }

        vector<string> initialParticipants;
        initialParticipants.push_back(getCurrentUsername());
        initialParticipants.push_back(secondUser);

        GroupChat* newGroup = new GroupChat(initialParticipants, groupName, getCurrentUsername());
        newGroup->setDescription(description);
        chats.push_back(newGroup);

        cout << "Group created successfully!\n";
    }

    void viewChats() const {
        if (!isLoggedIn()) {
            cout << "Please log in first!\n";
            return;
        }

        string current = getCurrentUsername();
        vector<Chat*> myChats;
        cout << "\n========================================\n";
        cout << "           Your Active Chats            \n";
        cout << "========================================\n";

        int count = 1;
        for (size_t i = 0; i < chats.size(); i++) {
            vector<string> participants = chats[i]->getParticipants();
            bool hasAccess = false;

            for (size_t j = 0; j < participants.size(); j++) {
                if (participants[j] == current) {
                    hasAccess = true;
                    break;
                }
            }

            if (hasAccess) {
                cout << count << ". " << chats[i]->getChatName() << "\n";
                myChats.push_back(chats[i]);
                count++;
            }
        }

        if (myChats.empty()) {
            cout << "No active chats found.\n========================================\n";
            return;
        }

        cout << "========================================\n";

        int choice;
        cout << "Enter chat number to open (or 0 to go back): ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice > 0 && choice <= static_cast<int>(myChats.size())) {
            myChats[choice - 1]->displayChat();
        }
        else if (choice != 0) {
            cout << "Access denied.\n";
        }
    }

    void logout() {
        if (isLoggedIn()) {
            users[currentUserIndex].updateLastSeen();
        }
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