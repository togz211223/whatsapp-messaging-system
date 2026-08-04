#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

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

    User(const string& uname, const string& pwd, const string& phone) {
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

    void setStatus(const string& newStatus) { status = newStatus; }
    void setPhoneNumber(const string& phone) { phoneNumber = phone; }

    void updateLastSeen() {
        time_t now = time(0);
        lastSeen = ctime(&now);
        if (!lastSeen.empty() && lastSeen.back() == '\n') {
            lastSeen.pop_back();
        }
    }

    bool checkPassword(const string& pwd) const { return pwd == password; }

    void changePassword(const string& newPwd) {
        password = newPwd;
        updateLastSeen();
    }
};

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

    Message(const string& sndr, const string& cntnt) {
        sender = sndr;
        content = cntnt;
        status = "sent";
        updateTimestamp();
        replyTo = nullptr;

        // FR13: Automatically convert text codes into emojis on creation
        addEmoji(":)");
        addEmoji(":(");
        addEmoji(":D");
        addEmoji("<3");
        addEmoji(":thumbsup:");
    }

    string getContent() const { return content; }
    string getSender() const { return sender; }
    string getTimestamp() const { return timestamp; }
    string getStatus() const { return status; }
    Message* getReplyTo() const { return replyTo; }

    void setStatus(const string& newStatus) { status = newStatus; }
    void setReplyTo(Message* msg) { replyTo = msg; }

    void updateTimestamp() {
        time_t now = time(0);
        timestamp = ctime(&now);
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
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

    void addEmoji(const string& emojiCode) {
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

class Chat {
protected:
    vector<string> participants;
    vector<Message> messages;
    string chatName;

public:
    Chat() {}

    Chat(const vector<string>& users, const string& name) {
        participants = users;
        chatName = name;
    }

    // Fix A: Added virtual destructor to handle polymorphic deletions safely
    virtual ~Chat() {}

    // Handles vector reallocation safely to prevent dangling replyTo pointers
    void addMessage(const Message& msg) {
        vector<Message*> oldAddresses;
        for (size_t i = 0; i < messages.size(); i++) {
            oldAddresses.push_back(&messages[i]);
        }

        messages.push_back(msg);

        // Remap reply pointers in case reallocation shifted memory locations
        for (size_t i = 0; i < messages.size() - 1; i++) {
            Message* reply = messages[i].getReplyTo();
            if (reply != nullptr) {
                int oldIdx = -1;
                for (size_t j = 0; j < oldAddresses.size(); j++) {
                    if (oldAddresses[j] == reply) {
                        oldIdx = static_cast<int>(j);
                        break;
                    }
                }
                if (oldIdx != -1) {
                    messages[i].setReplyTo(&messages[oldIdx]);
                }
            }
        }
    }

    // Safely handles deletion and vector memory-shifting to prevent dangling reply pointers
    bool deleteMessage(size_t index, const string& username) {
        if (index < messages.size()) {
            if (messages[index].getSender() == username) {
                Message* msgToDelete = &messages[index];

                vector<Message*> oldAddresses;
                for (size_t i = 0; i < messages.size(); i++) {
                    oldAddresses.push_back(&messages[i]);
                }

                // First pass: Null out replies directly pointing to the target message
                for (size_t i = 0; i < messages.size(); i++) {
                    if (messages[i].getReplyTo() == msgToDelete) {
                        messages[i].setReplyTo(nullptr);
                    }
                }

                messages.erase(messages.begin() + index);

                // Second pass: Adjust references because remaining vector elements shifted
                for (size_t i = 0; i < messages.size(); i++) {
                    Message* reply = messages[i].getReplyTo();
                    if (reply != nullptr) {
                        int oldIdx = -1;
                        for (size_t j = 0; j < oldAddresses.size(); j++) {
                            if (oldAddresses[j] == reply) {
                                oldIdx = static_cast<int>(j);
                                break;
                            }
                        }
                        if (oldIdx != -1) {
                            if (oldIdx == static_cast<int>(index)) {
                                messages[i].setReplyTo(nullptr);
                            }
                            else if (oldIdx > static_cast<int>(index)) {
                                messages[i].setReplyTo(&messages[oldIdx - 1]);
                            }
                            else {
                                messages[i].setReplyTo(&messages[oldIdx]);
                            }
                        }
                    }
                }
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

    vector<Message> searchMessages(const string& keyword) const {
        vector<Message> matches;
        for (size_t i = 0; i < messages.size(); i++) {
            if (messages[i].getContent().find(keyword) != string::npos) {
                matches.push_back(messages[i]);
            }
        }
        return matches;
    }

    vector<string> getParticipants() const { return participants; }
    string getChatName() const { return chatName; }

    Message* getMessagePtr(size_t index) {
        if (index < messages.size()) {
            return &messages[index];
        }
        return nullptr;
    }
};

class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(const string& u1, const string& u2) : Chat(vector<string>{u1, u2}, "Chat between " + u1 + " and " + u2) {
        user1 = u1;
        user2 = u2;
    }

    // Fix C: Removed unused getters getUser1() and getUser2() to eliminate dead-code complexity.

    void displayChat() const override {
        cout << "\n=== Private Chat ===\n";
        cout << user1 << " and " << user2 << endl;

        if (messages.empty()) {
            cout << "No messages yet.\n";
        }
        else {
            for (size_t i = 0; i < messages.size(); i++) {
                cout << "[" << i << "] ";
                messages[i].display();
                cout << "-----------------------\n";
            }
        }

        // FR15: Display typing indicator during private chat render
        showTypingIndicator(user2);
        cout << "-----------------------\n";
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

class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;

public:
    GroupChat(const vector<string>& users, const string& name, const string& creator) : Chat(users, name) {
        description = "";
        if (!isParticipant(creator)) {
            participants.push_back(creator);
        }
        admins.push_back(creator);
    }

    void addAdmin(const string& newAdmin) {
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

    bool isAdmin(const string& username) const {
        for (size_t i = 0; i < admins.size(); i++) {
            if (admins[i] == username) { return true; }
        }
        return false;
    }

    bool isParticipant(const string& username) const {
        for (size_t i = 0; i < participants.size(); i++) {
            if (participants[i] == username) { return true; }
        }
        return false;
    }

    void setDescription(const string& desc) {
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
                cout << "[" << i << "] ";
                messages[i].display();
                cout << "-----------------------\n";
            }
        }
    }

    void sendJoinRequest(const string& username) {
        cout << username << " requested to join group: " << chatName << endl;
    }
};

class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;

    int findUserIndex(const string& username) const {
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].getUsername() == username) { return static_cast<int>(i); }
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
            else { break; }
        }
        while (true) {
            cout << "Enter password: ";
            cin >> password;
            if (password.length() < 6) {
                cout << "Password must be at least 6 characters.\n";
            }
            else { break; }
        }
        while (true) {
            cout << "Enter phone number: ";
            cin >> phoneNumber;
            if (phoneNumber.length() != 11) {
                cout << "Phone number must be 11 digits.\n";
            }
            else { break; }
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

        // FR5: Update lastSeen upon successful login activity
        users[currentUserIndex].updateLastSeen();
        cout << "login succesfully...\n";
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
        cin.ignore(10000, '\n');
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
        cin.ignore(10000, '\n');
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

    void viewChats() {
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
            cin.ignore(10000, '\n');
            return;
        }
        cin.ignore(10000, '\n');

        if (choice > 0 && choice <= static_cast<int>(myChats.size())) {
            Chat* selectedChat = myChats[choice - 1];
            GroupChat* groupPtr = dynamic_cast<GroupChat*>(selectedChat);

            while (true) {
                selectedChat->displayChat();
                if (groupPtr != nullptr) {
                    cout << "\n[1] Send Message  [2] Reply  [3] Delete Message  [4] Search  [5] Manage Group  [0] Back\nChoice: ";
                }
                else {
                    cout << "\n[1] Send Message  [2] Reply  [3] Delete Message  [4] Search  [0] Back\nChoice: ";
                }

                int chatAction;
                if (!(cin >> chatAction)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    continue;
                }
                cin.ignore(10000, '\n');

                if (chatAction == 1) {
                    string text;
                    cout << "Enter message: ";
                    getline(cin, text);
                    Message newMsg(current, text);
                    selectedChat->addMessage(newMsg);

                    // FR5: Update lastSeen on sending message activity
                    users[currentUserIndex].updateLastSeen();
                }
                else if (chatAction == 2) {
                    cout << "Enter message index to reply to: ";
                    size_t idx;
                    cin >> idx;
                    cin.ignore(10000, '\n');
                    string text;
                    cout << "Enter reply message: ";
                    getline(cin, text);
                    Message newMsg(current, text);
                    newMsg.setReplyTo(selectedChat->getMessagePtr(idx));
                    selectedChat->addMessage(newMsg);
                    cout << "Message sent as reply!\n";

                    // FR5: Update lastSeen on replying activity
                    users[currentUserIndex].updateLastSeen();
                }
                else if (chatAction == 3) {
                    size_t idx;
                    cout << "Enter message index to delete: ";
                    cin >> idx;
                    cin.ignore(10000, '\n');
                    if (selectedChat->deleteMessage(idx, current)) {
                        cout << "Message deleted successfully.\n";

                        // FR5: Update lastSeen on deletion activity
                        users[currentUserIndex].updateLastSeen();
                    }
                    else {
                        cout << "Failed: Invalid index or you are not the sender.\n";
                    }
                }
                else if (chatAction == 4) {
                    string keyword;
                    cout << "Enter keyword to search: ";
                    cin >> keyword;
                    cin.ignore(10000, '\n');
                    vector<Message> results = selectedChat->searchMessages(keyword);
                    cout << "\n--- Search Results ---\n";
                    for (size_t i = 0; i < results.size(); i++) {
                        cout << "- " << results[i].getSender() << ": " << results[i].getContent() << "\n";
                    }

                    // FR5: Update lastSeen on search activity
                    users[currentUserIndex].updateLastSeen();
                }
                else if (chatAction == 5 && groupPtr != nullptr) {
                    if (!groupPtr->isAdmin(current)) {
                        cout << "Only admins can manage the group.\n";
                    }
                    else {
                        cout << "\n--- Group Management ---\n";
                        cout << "[1] Add Admin\n[2] Remove Participant\nChoice: ";
                        int mChoice;
                        cin >> mChoice;
                        cin.ignore(10000, '\n');
                        if (mChoice == 1) {
                            string u;
                            cout << "Enter username to make admin: ";
                            cin >> u;
                            cin.ignore(10000, '\n');
                            groupPtr->addAdmin(u);

                            // FR5: Update lastSeen on admin settings activity
                            users[currentUserIndex].updateLastSeen();
                        }
                        else if (mChoice == 2) {
                            string u;
                            cout << "Enter username to remove: ";
                            cin >> u;
                            cin.ignore(10000, '\n');
                            groupPtr->removeParticipant(current, u);

                            // FR5: Update lastSeen on participant removal activity
                            users[currentUserIndex].updateLastSeen();
                        }
                    }
                }
                else if (chatAction == 0) {
                    break;
                }
            }
        }
        else if (choice != 0) {
            cout << "Access denied.\n";
        }
    }

    // FR22: Send a request to join an existing group chat
    void requestToJoinGroup() {
        if (!isLoggedIn()) {
            cout << "Please log in first!\n";
            return;
        }
        string current = getCurrentUsername();
        vector<GroupChat*> availableGroups;

        for (Chat* chat : chats) {
            GroupChat* group = dynamic_cast<GroupChat*>(chat);
            if (group != nullptr && !group->isParticipant(current)) {
                availableGroups.push_back(group);
            }
        }

        if (availableGroups.empty()) {
            cout << "No joinable groups available.\n";
            return;
        }

        cout << "\n--- Request to Join Group ---\n";
        for (size_t i = 0; i < availableGroups.size(); i++) {
            cout << i + 1 << ". " << availableGroups[i]->getChatName() << "\n";
        }
        cout << "Select a group number to request to join (or 0 to go back): ";
        size_t choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }
        cin.ignore(10000, '\n');

        if (choice > 0 && choice <= availableGroups.size()) {
            availableGroups[choice - 1]->sendJoinRequest(current);
            users[currentUserIndex].updateLastSeen();
        }
    }

    // FR4 & FR6: View, modify profile properties (Status/Phone), and update Password
    void manageProfile() {
        if (!isLoggedIn()) {
            cout << "Please log in first!\n";
            return;
        }
        while (true) {
            cout << "\n--- Settings & Profile ---\n";
            cout << "Current Status: " << users[currentUserIndex].getStatus() << "\n";
            cout << "Current Phone:  " << users[currentUserIndex].getPhoneNumber() << "\n";
            cout << "Last Seen:      " << users[currentUserIndex].getLastSeen() << "\n\n";
            cout << "1. Update Status\n";
            cout << "2. Update Phone Number\n";
            cout << "3. Change Password\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(10000, '\n');

            if (choice == 1) {
                string newStatus;
                cout << "Enter new status: ";
                getline(cin, newStatus);
                users[currentUserIndex].setStatus(newStatus);
                users[currentUserIndex].updateLastSeen();
                cout << "Status updated successfully!\n";
            }
            else if (choice == 2) {
                string newPhone;
                cout << "Enter new phone number (11 digits): ";
                cin >> newPhone;
                cin.ignore(10000, '\n');
                if (newPhone.length() != 11) {
                    cout << "Phone number must be exactly 11 digits!\n";
                }
                else {
                    users[currentUserIndex].setPhoneNumber(newPhone);
                    users[currentUserIndex].updateLastSeen();
                    cout << "Phone number updated successfully!\n";
                }
            }
            else if (choice == 3) {
                string oldPwd, newPwd;
                cout << "Enter current password: ";
                cin >> oldPwd;
                if (!users[currentUserIndex].checkPassword(oldPwd)) {
                    cout << "Incorrect password.\n";
                }
                else {
                    cout << "Enter new password (min 6 characters): ";
                    cin >> newPwd;
                    if (newPwd.length() < 6) {
                        cout << "Password must be at least 6 characters.\n";
                    }
                    else {
                        users[currentUserIndex].changePassword(newPwd);
                        cout << "Password changed successfully!\n";
                    }
                }
            }
            else if (choice == 0) {
                break;
            }
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
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    continue;
                }
                if (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) break;
            }
            else {
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Request to Join Group\n5. Settings & Profile\n6. Logout\nChoice: ";
                int choice;
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    continue;
                }
                if (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) requestToJoinGroup();
                else if (choice == 5) manageProfile();
                else if (choice == 6) logout();
            }
        }
    }
};

int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}
