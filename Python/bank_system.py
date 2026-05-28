import json
import random

class Account:

    def __init__(self, username, pin, balance=0, account_no=None, history=None):
        self.username = username
        self.__pin = pin
        self.balance = balance

        self.account_no = account_no if account_no else self.generate_account_no()

        self.history = history if history else []
        
    def generate_account_no(self):
        return str(random.randint(10000000, 99999999))

    def verify_pin(self, pin):
        return self.__pin == pin

    def deposit(self, amount):
        if amount <= 0:
            print("Invalid Amount")
            return
        self.balance += amount
        self.history.append(f"Deposited ₹{amount}")
        print(f"₹{amount} Deposited Successfully")

    def withdraw(self, amount):
        if amount <= 0:
            print("Invalid Amount")
            return
        if amount > self.balance:
            print("Insufficient Balance")
            return
        self.balance -= amount
        self.history.append(f"Withdraw ₹{amount}")
        print(f"₹{amount} Withdrawn Successfully")

    def transfer(self, receiver, amount):
        if amount <= 0:
            print("Invalid Amount")
            return
        if amount > self.balance:
            print("Insufficient Balance")
            return
        self.balance -= amount
        receiver.balance += amount
        self.history.append(
            f"Transferred ₹{amount} to {receiver.username}"
        )
        receiver.history.append(
            f"Received ₹{amount} from {self.username}"
        )
        print("Transfer Successful")

    def view_balance(self):
        print(f"Current Balance : ₹{self.balance}")

    def show_history(self):
        print("\n===== TRANSACTION HISTORY =====")
        if not self.history:
            print("No Transactions")
        else:
            for transaction in self.history:
                print(transaction)

    def to_dict(self):
        return {
            "username": self.username,
            "pin": self.__pin,
            "balance": self.balance,
            "account_no": self.account_no,
            "history": self.history
        }

class Bank:

    def __init__(self):
        self.accounts = {}
        self.load_data()

    def create_account(self):
        username = input("Enter Username: ")
        if username in self.accounts:
            print("Username Already Exists")
            return
        pin = input("Create 4-Digit PIN: ")
        account = Account(username, pin)
        self.accounts[username] = account
        self.save_data()
        print("Account Created Successfully")
        print(f"Account Number: {account.account_no}")

    def login(self):
        username = input("Enter Username: ")
        pin = input("Enter PIN: ")
        account = self.accounts.get(username)
        if account and account.verify_pin(pin):
            print(f"\nWelcome {username}")
            self.account_menu(account)
        else:
            print("Invalid Username or PIN")

    def account_menu(self, account):
        while True:
            print("\n===== ACCOUNT MENU =====")
            print("1. Deposit")
            print("2. Withdraw")
            print("3. Transfer Money")
            print("4. View Balance")
            print("5. Transaction History")
            print("6. Delete Account")
            print("7. Logout")

            choice = input("Enter Choice: ")

            if choice == "1":
                amount = float(input("Enter Amount: "))
                account.deposit(amount)
                self.save_data()

            elif choice == "2":
                amount = float(input("Enter Amount: "))
                account.withdraw(amount)
                self.save_data()

            elif choice == "3":
                receiver_name = input("Enter Receiver Username: ")
                receiver = self.accounts.get(receiver_name)
                if not receiver:
                    print("Receiver Not Found")
                else:
                    amount = float(input("Enter Amount: "))
                    account.transfer(receiver, amount)
                    self.save_data()

            elif choice == "4":
                account.view_balance()

            elif choice == "5":
                account.show_history()

            elif choice == "6":
                confirm = input(
                    "Are you sure? (yes/no): "
                )
                if confirm.lower() == "yes":
                    del self.accounts[account.username]
                    self.save_data()
                    print("Account Deleted")
                    break

            elif choice == "7":
                print("Logged Out")
                break
            
            else:
                print("Invalid Choice")

    def save_data(self):
        data = {}
        for username, account in self.accounts.items():
            data[username] = account.to_dict()
        with open("bank_data.json", "w") as file:
            json.dump(data, file, indent=4)

    def load_data(self):
        try:
            with open("bank_data.json", "r") as file:
                data = json.load(file)
                for username, info in data.items():
                    account = Account(
                        info["username"],
                        info["pin"],
                        info["balance"],
                        info["account_no"],
                        info["history"]
                    )
                    self.accounts[username] = account

        except FileNotFoundError:
            pass


bank = Bank()

while True:

    print("\n========= PRINCE SMART BANK =========")
    print("1. Create Account")
    print("2. Login")
    print("3. Exit")

    choice = input("Enter Choice: ")

    if choice == "1":
        bank.create_account()

    elif choice == "2":
        bank.login()

    elif choice == "3":
        print("Thank You For Using Prince Smart Bank")
        break

    else:
        print("Invalid Choice")