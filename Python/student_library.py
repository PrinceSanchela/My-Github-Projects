class Student():
    
    def __init__(self,studentN):
        self.studentN = studentN   
        self.studentBookList = []
        
    def viewBook(self):
        if self.studentBookList:
            print(f"This is {self.studentN}'s Books : ")
            print(self.studentBookList)
        else:
            print(f"No Purchased Book in {self.studentN}'s BookList")
    
    def purchaseBook(self,bookName):
        
        if bookName in bookList and bookName not in self.studentBookList:
            
            print("Your Book is Successfully Found into Library ....")
            self.studentBookList.append(bookName)
            print(f"Your Book is Successfully Purchased By You and added to {self.studentN}'s BookList")
            return True
            
        elif bookName in self.studentBookList:
            print("This Book is already Purchased By You in Your BookList")
            return False
            
        else:
            print("This Book is not in Library")
            return False
    
bookList = ['Physics','Maths','Science','English']

studentList = {
    "Prince": Student("Prince"),
    "Rahul": Student("Rahul")
}

def login():
    print("\nNow You are into Library Login Page")
    name = input("Enter Your Name : ").capitalize()
    if name in studentList:
        print("\n You are Successfully Login into Library")
    else:
        print("You are not in Student Account of This Library Account")
        studentList.update({name: Student(name)})
        print(f"Library is auto Created your Student Account named '{name}'")
    
    return studentList[name]
        
        
print("Welcome to Prince Library ... ")    
print("\nNow You are Into Prince Library And These are The Books in this Library\n")
print(bookList)

while True:
            
    print("1. Purchase Book \n2. Login into Library as Student \n3. Exit to Library")
    choice = input("Enter Your choice : ")
    if choice == '1':
        name = login()
        while True:
            bookName = input("Enter The Book Name : ").capitalize()
            if not name.purchaseBook(bookName):
                continue
            else:
                break
            
    elif choice == '2':
        name = login()
        while True:
            print("1. View Your Book \n2. Log Out")
            choice = input("Enter Your Choice : ")
            if choice == '1':
                name.viewBook()
            elif choice == '2':
                break
            else:
                print("Enter Valid Option...")
                continue
    elif choice == '3':
        break
    else:
        print("Please Enter Valid Option!!")
        continue    