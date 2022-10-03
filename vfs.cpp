#include <iostream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <ctime>
#include <string>
#include <algorithm>
#include <fstream>


using namespace std;
using std::string;

string giveDate();
class Inode;
class pathStack;


bool findFlag;

bool string_is_valid(const std::string &str)
{
    return find_if(str.begin(), str.end(), 
    [](char c) { return !(isalnum(c) || (c == ' ')); }) == str.end();
}

bool file_string_is_valid(const std::string &str)
{	//if  return false

    return (str.find(".")!=-1) || find_if(str.begin(), str.end(), 
    [](char c) 
    { return !(isalnum(c) || (c == ' ')); }) == str.end();
}


// create linked tree class
class Vfstree
{
private:
	Inode *root;
private:
	Inode* insert();
public:
	Vfstree();
	Inode* getRoot();
	void addDirectory(Inode* , string); 
	void listDirectory(Inode*);
	void changeDirectory(Inode*&, string);
	void addFile(Inode*, string, string);
	string printDirectory(Inode*, pathStack&);
	int PostOrderSize(Inode*);
	void  findFile(Inode*, string, pathStack&);
	void findAll(Inode* , string);
	int findSize(Inode*, string);
	void removeInode(Inode*, string&);
	Inode* pathToInode(Inode*, string);
	void moveFile(Inode* , string , string );
	void dumpfile(ofstream& , Inode* );
	void loadfile(ifstream&);

}files;

class trashBinList
{
	Inode* head;
	Inode* tail;
	int sizecount;
	int MAXCOUNT = 10;
	friend class Vfstree;
public:
	trashBinList();
	void addBack(Inode*);
	void displayBin();
	void emptyBin();
}bin;


class InodeList
{
private:
	Inode* head;
	Inode* tail;
	friend class Vfstree;
public:
	InodeList();
	void addBack(Inode* v);
	void displayChildren();	
	Inode* searchChildren(string);
};

// create inode class
class Inode
{
private:
	string name;
	bool isFolder;
	int size;
	string date;
	string binpath;
private:
	Inode *next = NULL;
	Inode *prev = NULL; 
private:
	Inode *parent;
	InodeList *children;
	friend class InodeList;
	friend class trashBinList;
public:
	friend class Vfstree;
	Inode(bool, string, int);
} *currentprev;

class pathStack
{
private: 
	string *name;
	int capacity;
	int t;

public: 
	pathStack()
	{
		capacity = 500;
		name = new string[capacity];
		t = -1;
	}

	bool empty() const;
	string& top() const;
	void push(const string& e);
	string& pop();
};

bool pathStack::empty() const
{ return (t<0); } 

string& pathStack::top() const 
{
	if (empty()) cout << "\n Stack is empty";
	return name[t];
}

void pathStack::push(const string& e)
{
	name[++t] = e;
}

string& pathStack::pop() 
{
	if (empty()) cout << "\n Pop from empty stack ";
	return name[--t];
}

pathStack path;

trashBinList::trashBinList()
{	
	sizecount = 0;
	head = new Inode(false, "", 0);
	tail = new Inode(false, "", 0);
	head -> next = tail;
	tail -> prev = head;
}

void trashBinList::addBack(Inode* v)
{
	++sizecount;
	v -> next = tail;
	v -> prev = tail -> prev;
	tail -> prev = v;
	v -> prev -> next = v;
}

void trashBinList::emptyBin()
{	
	sizecount = 0;
	head -> next = tail;
	tail -> prev = head;
}

Vfstree::Vfstree()
{
	root = new Inode(true, "", 0);
	root -> parent = NULL;
}

Inode* Vfstree::getRoot()
{
	return root;
}

void Vfstree::addDirectory(Inode* current, string name)
{
	Inode *newdir = new Inode(true, name, 10);
	newdir -> parent = current;
	current->children->addBack(newdir);
}

void Vfstree::listDirectory(Inode* current)
{
	if (current->isFolder==true)
		current->children->displayChildren();
}

Inode* Vfstree::pathToInode(Inode* current, string path)
{
	string target;
		string parameter = path;
	if (path.front()==*"/")
	{
		while (parameter.find("/")!=-1)
		{	
			parameter = parameter.substr(parameter.find("/")+1);
			target = parameter.substr(0,parameter.find("/"));
			current = pathToInode(current, target);
		}

	}

	if (current->isFolder == false)  
		{
			cout << " Specified target is a file! " << endl;
			return current->parent;
		}
		
	if ((current->isFolder == true)&&(parameter!=target))
	{
		bool notfound = false;
		Inode* targetPtr = current->children->searchChildren(path);
			if (targetPtr == nullptr)
				{
					notfound = true;
				}
			else if (targetPtr->name == path)
			{
				return targetPtr;
			}

		targetPtr = getRoot()->children->searchChildren(path);
			if (targetPtr == nullptr)
				{
					notfound = true;
				} 
			else if (targetPtr->name == path)
			{
				return targetPtr;
			}

		if (notfound == true)
			cout << " Directory not found! " << endl;
		
	}
	return current;		
}	

void Vfstree::changeDirectory(Inode* &current, string name)
{
	
	if (name=="..")
	{
		if (current->parent == NULL)
			cout << " Cannot go to parent directory!"<<endl;
		else 
			{	currentprev = current;
				current = current->parent;
			}
	}
	else if ((name == "cd")||(name == ""))
	{
		currentprev = current;
		current = getRoot();
	}
	else if (name=="-")
	{
		current = currentprev;
	}
	else
	{
		if (name.front()==*"/")
		{
			currentprev = current;
			Inode* newNode = pathToInode(current, name);
			current = newNode;

		}
		else
		{
			Inode* targerPtr = current->children->searchChildren(name);
			if (targerPtr == nullptr)
				{
					cout << " Directory does not exist!"<<endl;
					return;
				} 
			else if (! targerPtr->isFolder) cout << "Cannot change directory to file!"<<endl;
			else 
				{
					currentprev = current;
					current = targerPtr;
				}
		}
	}
}

void Vfstree::addFile(Inode* current, string name, string size)
{
	Inode *newfile = new Inode(false, name, stoi(size));
	newfile -> parent = current;
	current->children->addBack(newfile);
}


string Vfstree::printDirectory(Inode* current, pathStack& path)
{
	string temppath = "/";

	while (current->parent != NULL)
	{
		path.push(current->name);
		path.push("/");
		current = current -> parent;
	}
 
	while (!path.empty())
	{
		temppath +=path.pop();
	}

	string fullpath = temppath;
	return (fullpath);
	
}

void trashBinList::displayBin()
{	
	pathStack binpath;
	if (sizecount == 0)
	{
		cout << " Bin is empty! " << endl;
		return;
	}

	Inode *curr=head->next;
	
		string type = (curr->isFolder)?("Folder"):("File");
		cout<< setw(12) << std::left << type
				<<setw(15) << std::left <<curr->name
				<<setw(10)<<curr->size
				<<setw(18)<<curr->date<<endl;
		cout << curr->binpath <<endl;
}
 
int Vfstree::findSize(Inode* current, string name)
{
	if ((current->name == name)||(name == "size")||(name == ""))
		return PostOrderSize(current);
	if (current->isFolder)
	{
		current = current->children->head->next;
		while (current->next != NULL)
		{
			if (current->name == name)
				return PostOrderSize(current);
			current = current -> next;
		}
	}
	return -1;

}

int Vfstree::PostOrderSize(Inode* current)
{	 
	int s = current->size;

	if (current->isFolder == false)
	{
		return current->size;
	}
	else 
	{	
		current = current->children->head->next;
		while (current->next != NULL)
		{
			s += PostOrderSize(current);
			current = current -> next;
		}	
		return s;
	}
	// return s;
}

void  Vfstree::findFile(Inode* current, string name, pathStack& path)
{
  	Inode* file = current->children->searchChildren(name);

  	if(file == nullptr)
  	{
  		cout << " File does not exist in directory! " << endl;
  	}
  	else if (file->isFolder == true)
  	{
  		cout << " Please enter a file name! " << endl;
  	}
  	else
  	{
  		string realpath = printDirectory(file, path);
  		cout << realpath << endl;

  	}

}

void Vfstree::findAll(Inode* current, string name)
{
	if (current->name == name)
	{
		findFlag = true;
		if (current->isFolder)
			cout << printDirectory(current, path) << endl;
		else
		{
			findFile(current->parent, name, path);
		}

	}
	if (current->isFolder)
	{
		current = current->children->head->next;
		while (current->next != NULL)
		{
			findAll(current, name);
			current = current -> next;
		}	
	}
	
}

void Vfstree::removeInode(Inode* current, string& name)
{
	if (name=="")
	{
		cout << " Cannot delete root! " << endl;
		return;
	}
	if (bin.sizecount==bin.MAXCOUNT)
	{
		cout << "Bin is full! Please empty the bin." << endl;
		return;
	}

	bool found = false;
	Inode* temp = current;
	temp = current->children->head;
		while (temp->next != NULL)
		{
			if (temp->name == name)
			{

				found = true;

				Inode* bintemp = temp;	
				bintemp->binpath = printDirectory(bintemp, path);
				Inode* newprev = temp -> prev;
				Inode* newnext = temp -> next;

				newprev ->next = newnext;
				newnext-> prev = newprev;

				bin.addBack(bintemp);
				break;
			}
			temp = temp -> next;
		}
		if (found == false) 
			cout << " File or Folder does not exist! " << endl;
}

void Vfstree::moveFile(Inode* current, string target, string destination)
{
	if (destination.front()!=*"/")
	{
		cout << " Enter the path correctly! " << endl;
		return;
	}
	bool found = false;

	Inode* temp = current;
	temp = current->children->head;
		while (temp->next != NULL)
		{
			if (temp->name == target)
			{
				Inode* Inodedestination;
				Inode* newprev = temp -> prev;
				Inode* newnext = temp -> next;

				newprev ->next = newnext;
				newnext-> prev = newprev;

				// Inode* temproot = files.getRoot();
				Inodedestination = pathToInode(files.getRoot(), destination);

				Inodedestination->children->addBack(temp);
				temp->parent = Inodedestination;

				found = true;
				break;
			}
			temp = temp -> next;
		}
		if (found == false) 
			cout << " Target File or Folder does not exist! " << endl;
}


void Vfstree::dumpfile(ofstream& loadout, Inode* current)
{
	loadout << printDirectory(current, path) << "," << current->size << "," << current->date << endl;
	if (current->isFolder == true) 
	{	
		current = current->children->head->next;
		while (current->next != NULL)
		{
			dumpfile(loadout, current);
			current = current -> next;
		}	
		return;
	}
}


void Vfstree::loadfile(ifstream& loadin)
{
	if (!loadin.is_open())
		return;


	string line;

	string name, path, size, date, folderName;
	Inode* ptr;
	Inode* currentChild;

	loadin>>line;

		while (getline(loadin, line))
		{
			ptr = getRoot();
			stringstream ss(line);
			getline(ss, path, ',');
			getline(ss, size, ',');
			getline(ss,date, '\n');


			// string name = path.substr(path.rfind("/")+1);

			if (path == "/")
			{
				// cout << "root created" << endl;
				this->root->date = date;
				this->root->size = 0;
				continue;
			}
			
			while (path.find("/") != -1)
			{
				folderName = path.substr(0, path.find("/"));
				path = path.substr(path.find("/")+1);


				if (folderName=="")
					continue;
				// currentChild = ptr->children->head->next;
				// while (currentChild->next != NULL)		
				// {
				// 	if (currentChild->name==folderName)
				// 	{
				// 		return true;
				// 	}
				// }

				// return false;

				if (ptr->children->searchChildren(folderName) != nullptr )
				{
					ptr = ptr->children->searchChildren(folderName);
				}
				else 
				{
					this->addDirectory(ptr, folderName);
					ptr = ptr->children->searchChildren(folderName);

				}

			}


			if (path!="")
			{			
				if ((path.find(".")!=-1))
			{
				this->addFile(ptr, path, size);
			}
			else 
			{
				this->addDirectory(ptr, path);
			}
		}
				
		}
	loadin.close();
}

Inode::Inode(bool checkfolder, string name, int size)
{
	isFolder = checkfolder;
	if (!isFolder)
		children = NULL;
	else if (isFolder)
		children = new InodeList;
	this->name = name;
	this->size = size; 	
	date = giveDate();
	binpath = "";
}

InodeList::InodeList()
{
	head = new Inode(false, "", 0);
	tail = new Inode(false, "", 0);
	head -> next = tail;
	tail -> prev = head;
}

void InodeList::displayChildren()
{
	Inode *curr=head->next;
	while(curr->next!=NULL)
	{
		string type = (curr->isFolder)?("Folder"):("File");
		cout<< setw(12) << std::left << type
				<<setw(15) << std::left <<curr->name
				<<setw(10)<<curr->size
				<<setw(18)<<curr->date<<endl;
		curr=curr->next;
	}
	
}

Inode* InodeList::searchChildren(string name)
{
	Inode *curr=head->next;
	while(curr->next!=NULL)
	{
		if (name == curr->name)
			return curr;
		curr=curr->next;
	}
	return nullptr;
}

void InodeList::addBack(Inode* v)
{
	v -> next = tail;
	v -> prev = tail -> prev;
	tail -> prev = v;
	v -> prev -> next = v;
}



string giveDate()
{
	time_t curr_time;
	tm * curr_tm;
	char date_string[10];

	time(&curr_time);
	curr_tm = localtime(&curr_time);
	
	strftime(date_string, 9, "%d-%m-%C", curr_tm);
	
	return date_string;
}


void listOfCommands()
{
	cout<<"--------------------Virtual-File-System-------------------"<<endl;
	cout<<"pwd                          :Print path of the current node"<<endl
		<<"realpath {filename}          :Print full path of given file name"<<endl
		<<"ls                           :Display all items in a directory"<<endl
		<<"mkdir {foldername}           :Create a folder in a directory"<<endl
		<<"touch {filename} {size}      :Find the node with minimum key in BST"<<endl
		<<"cd {foldername}              :Change directory"<<endl
		<<"cd ..                        :Change to parent directory"<<endl
		<<"cd -                         :Change to previous directory"<<endl
		<<"cd {path}                    :Change to specified path"<<endl
		<<"cd                           :Change to root directory"<<endl
		<<"find {file/folder name}      :Print path of specified file or folder"<<endl
		<<"mv {filename} {foldername}   :Move a file to a specified folder"<<endl
		<<"rm {file/folder name}        :Delete a file or folder"<<endl
		<<"size {file/folder name}      :Print the size of a file or directory"<<endl
		<<"emptybin                     :Clear the bin file"<<endl
		<<"showbin                      :Show the oldest file in the bin"<<endl
		<<"help                         :Display the available commands"<<endl
		<<"exit                         :Exit the program"<<endl;
	cout<<"----------------------------------------------------------"<<endl;

}


void demo(Vfstree& tree) {
	Inode* ptr = tree.getRoot();

	cout<<"Debug"<<endl;

	tree.addDirectory(ptr, "OS");
	tree.changeDirectory(ptr, "OS");
	tree.addFile(ptr, "Windows", "100");
	tree.addFile(ptr, "BIOS", "30");
	tree.changeDirectory(ptr, "..");


	tree.addDirectory(ptr, "System");
	tree.changeDirectory(ptr, "System");
	tree.addDirectory(ptr, "Users");
	tree.changeDirectory(ptr, "Users");
	tree.addDirectory(ptr, "Soumen");
	tree.addDirectory(ptr, "Guest");
	tree.changeDirectory(ptr, "Guest");

	tree.addFile(ptr, "Data", "25");
	tree.changeDirectory(ptr, "..");
	tree.changeDirectory(ptr, "Soumen");
	tree.addDirectory(ptr, "Desktop");
	tree.changeDirectory(ptr, "Desktop");
	tree.addDirectory(ptr, "Classes");
	tree.addDirectory(ptr, "Music");
	tree.changeDirectory(ptr, "Music");
	tree.addFile(ptr, "Queens", "60");
	tree.changeDirectory(ptr, "..");
	tree.changeDirectory(ptr, "Classes");
	tree.addDirectory(ptr, "Class1");
	tree.addDirectory(ptr, "Class2");
	tree.addDirectory(ptr, "Class3");

}



int main()
{
	listOfCommands();
	string user_input;
	string command;
	string parameter, parameter2;

	ifstream loadin("vfs.dat");
	files.loadfile(loadin);

	Inode* current = files.getRoot();

	do
	{
		findFlag = false;

		cout<<">";
		getline(cin,user_input);

		command = user_input.substr(0,user_input.find(" "));
		parameter = user_input.substr(user_input.find(" ")+1);
		parameter2 = parameter.substr(parameter.find(" ")+1);
		parameter = parameter.substr(0,parameter.find(" "));

		//cout <<endl <<"$"<<command<<"$"<<parameter<<"$"<<parameter2<<"$"<<endl;

		try
		{
			
			if(command=="mkdir")
			{
				if (string_is_valid(parameter))
					files.addDirectory(current, parameter);
				else cout << " Name can only have alphanumeric characters! " << endl;
			}

			else if(command=="ls")				files.listDirectory(current);
			else if(command=="cd")				files.changeDirectory(current, parameter);
			else if(command=="touch")
			{
				if (file_string_is_valid(parameter))
					files.addFile(current, parameter, parameter2);
				else cout << " Name can only have alphanumeric characters! " << endl;
			}			
			else if(command=="pwd")				cout << files.printDirectory(current, path) << endl;
			else if(command=="size")			
					{	int sizevar = -1;
						sizevar = files.findSize(current, parameter);
						if (sizevar == -1)
							cout << " File or Folder does not exist! " << endl;
						else
							cout << sizevar << endl;
					}
			else if(command=="demo")			demo(files);
			else if(command=="realpath")		files.findFile(current, parameter, path);
			else if(command=="find")			
												{
													files.findAll(files.getRoot(), parameter);
													if (findFlag==false) cout << "The entered file or folder does not exist! " << endl;
												}
			else if(command=="help")			listOfCommands();
			else if(command=="rm")				files.removeInode(current, parameter);
			else if(command=="showbin")			bin.displayBin();
			else if(command=="emptybin")		bin.emptyBin();
			else if(command=="mv")				files.moveFile(current, parameter, parameter2);
			else if((command=="exit")||(command=="quit"))				break;

			else 	
				cout<<"Invalid Command! "<<endl;
		}
		catch(exception &e)
		{
			cout<<"Exception: "<<e.what()<<endl;
		}

	}while(command!="exit" and command!="quit");

	cout << "Saving file ... " << endl;
	ofstream loadout("vfs.dat");
	files.dumpfile(loadout, files.getRoot());
	loadout.close();

	return EXIT_SUCCESS;
}

