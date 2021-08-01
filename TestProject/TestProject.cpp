#include <iostream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

struct ListNode {
    ListNode *      prev;
    ListNode *      next;
    ListNode *      rand; // указатель на произвольный элемент данного списка либо NULL
    string     data;
};

class List {
public:
    ~List();

    void Serialize   (FILE * file);  // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
    void Deserialize (FILE * file);  // загрузка из файла (файл открыт с помощью fopen(path, "rb"))
    
    void Show() const;
    void Add(string data);
    ListNode* GetNodeByIndex(int index) const;
    void AssignRandom();

    private:
    ListNode    *head=nullptr;
    ListNode    *tail=nullptr;
    int         count=0;
};

List::~List()
 {
    while (head)
        {
            tail = head->next;
            delete head;
            head = tail;
        }
 }

void List::Serialize(FILE* file)
{
    if(fopen_s(&file, "serializeddouble.bin","wb")!=0)
    {
        cout << "Can't open file for writing" << endl;
        return;
    }

    //Save nodes count
    fwrite(&count, sizeof(count), 1, file);
    
    ListNode *loopNode = head;
    while (loopNode != nullptr)
    {
        //Save node data character count
        int stringSize = static_cast<int>(loopNode->data.size());
        fwrite(&stringSize,sizeof(stringSize), 1 ,file);

        //Save node data
        fwrite(loopNode->data.c_str(),1, stringSize ,file);

        //Get current node rand index
        ListNode* randNode = loopNode->rand;
        int randCounter = -1;
        
        while (randNode)
        {
            randNode = randNode->prev;
            randCounter++;
        }

        //Save node rand index
        fwrite(&randCounter, sizeof randCounter, 1, file);
        
        fflush(file);

        loopNode = loopNode->next;
    }

    fclose(file);
}

void List::Deserialize(FILE* file)
{
    if(fopen_s(&file, "serializeddouble.bin","rb")!=0)
    {
        cout << "Can't open file for reading" << endl;
        return;
    }

    //Get node count
    int listLength = 0;
    fread(&listLength, sizeof(listLength), 1 , file);

    //Used to save rands for a second pass
    stack<int> randStack; 

    //Used by rand assigning to get nodes by index
    std::vector<ListNode*> nodes = {}; 
    
    for (int i = 0; i < listLength; ++i)
    {
        //Get node data character count
        int newStringLength = 0;
        fread(&newStringLength,sizeof(newStringLength),1,file);

        //Get node data
        string newString(newStringLength, 0);
        fread(&newString[0],1,newStringLength,file);
        
        Add(newString);

        //Save node for rand assigning
        nodes.push_back(tail);

        //Get node rand index
        int randomIndex = -1;
        fread(&randomIndex, sizeof(randomIndex), 1, file);
        
        randStack.push(randomIndex);
    }

    //Assigning rands
    ListNode* loopNode = tail;
    while(!randStack.empty()&&loopNode!=nullptr)
    {
        loopNode->rand = randStack.top()==-1?nullptr:nodes[randStack.top()];
        randStack.pop();
        loopNode = loopNode->prev;
    }

    fclose(file);
}

void List::Add(const string data)
{
    ListNode *newNode = new ListNode;
    newNode->next = nullptr;
    newNode->rand = nullptr;
    newNode->data = data;
 
    if (head != nullptr)
    {
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
    }
    else
    {
        newNode->prev = nullptr;
        head = tail = newNode;
    }

    count++;
}

ListNode* List::GetNodeByIndex(const int index) const
{
    if(index>=count){return nullptr;}
    ListNode* indexNode = head;
    for (int i = 0; i < index; ++i)
    {
        indexNode = indexNode->next;
    }

    return indexNode;
}

void List::AssignRandom()
{
    ListNode *loopNode = head;
    
    // srand(time(nullptr));
    srand(2);
    
    while (loopNode != nullptr)
    {
        //Rng in range count
        const int rng = rand() % count;

        ListNode* rngNode = GetNodeByIndex(rng);
        if(!rngNode){continue;}
        
        loopNode->rand = rngNode;
        
        loopNode = loopNode->next;
    }
}


void List::Show() const
{
    cout << "Forward: " <<endl;
    ListNode *loopNode = head;
    while (loopNode != nullptr)
    {
        cout << "Data: "<< loopNode->data << " ";
        if(loopNode->rand)
        {
            cout << "Random: "<< loopNode->rand->data;
        }
        loopNode = loopNode->next;
        cout <<endl;
    }
    cout << "-----------------------------------" << endl;
    
    cout << "Backwards: " << endl;
    
    loopNode=tail;
    while (loopNode != nullptr)
        {
        cout << "Data: " << loopNode->data << " ";
        if(loopNode->rand)
        {
            cout << "Random: " << loopNode->rand->data;
        }
        loopNode = loopNode->prev;
        cout << endl;
        }
}

void RemoveDups(char* pStr)
{
    if(!pStr||strlen(pStr)==0){return;}

    int writeIndex = 0;
    
    for (int readIndex = 0; readIndex < strlen(pStr); readIndex++)
    {
        if(pStr[readIndex] != pStr[writeIndex])
        {
            writeIndex++;
            pStr[writeIndex] = pStr[readIndex];
        }
    }
    
    //End string
    writeIndex++;
    pStr[writeIndex] = '\0';
}

int main(int argc, char* argv[])
{
    cout << "-----------Dups removal------------" << endl;
    char inputText[] = "AAA BBB AAA CC MMNS LLLL DD X";
    
    std::cout << "Input: " << inputText << endl;
    RemoveDups(inputText);
    std::cout << "Cleaned string: " << inputText << endl;

    

    cout << "-----------Current list------------" << endl;
    List* origList = new List();
    origList->Add("Cake");
    origList->Add("Pain");
    origList->Add("Create");
    origList->Add("Apple");
    origList->Add("01101");
    origList->Add("Arpeggio's");

    origList->AssignRandom();
    
    origList->Show();

    FILE* filePtr=nullptr;
    
    origList->Serialize(filePtr);
    
    cout << "---------Deserialized list---------" << endl;

    List* loadedList = new List();
    loadedList->Deserialize(filePtr);
    loadedList->Show();
    
    return 0;
}
