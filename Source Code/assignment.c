#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include "bus_reservation.h"

// Define various constants to be used throughout the program
#define MAX_LINE 512              // Maximum line length for input/output
#define MAX_USERS 100             // Maximum number of users in the system
#define USERNAME_LENGTH 50        // Maximum length for a username
#define PASSWORD_LENGTH 50        // Maximum length for a password
#define EMAIL_LENGTH 100          // Maximum length for an email
#define PHONE_LENGTH 20           // Maximum length for a phone number
#define ADDRESS_LENGTH 200        // Maximum length for an address
#define MAX_ATTEMPTS 3            // Maximum login attempts before lockout
#define MAX_BUSES 100             // Maximum number of buses in the system
#define MAX_SEATS 50              // Maximum number of seats per bus
#define MAX_BOOKINGS 100           // Maximum bookings per user
#define SEATS_PER_ROW 4           // Number of seats per row in a bus
#define SST_RATE 0.06             // Sales and Service Tax (SST) rate (6%)
#define MAX_LENGTH 256            // General maximum string length
#define MAX_NOTIFICATIONS 100     // Maximum number of notifications stored

// Structure to store bus reservation details
struct BusReservation {
    int busID;                        // Unique identifier for each bus
    char busNumberPlate[20];           // Bus number plate
    char date[11];                     // Date of travel (Format: YYYY-MM-DD)
    char source[50];                   // Starting location
    char destination[50];               // Destination location
    char departureTime[20];             // Departure time (Format: HH:MM AM/PM)
    char arrivalTime[20];               // Arrival time (Format: HH:MM AM/PM)
    int totalSeats;                     // Total seats available on the bus
    int availableSeats;                 // Seats available for booking
    float fare;                         // Ticket price per seat
    int reservedSeats[MAX_SEATS];       // Array storing reserved seat numbers
    int reservedCount;                  // Count of reserved seats
    int bookings[MAX_BOOKINGS][MAX_SEATS];  // 2D array to store seat numbers per booking
    int ticketNumbers[MAX_BOOKINGS];    // Array to store unique 6-digit ticket numbers
    int seatCounts[MAX_BOOKINGS];       // Number of seats booked per booking
    int bookingCount;                   // Total number of bookings made
    int totalBookings;                  // Total successful bookings
    int totalCancellations;             // Total number of canceled bookings
    float totalRevenue;                 // Total revenue generated from bookings
    float lostRevenue;                  // Revenue lost due to cancellations
    float netRevenue;                   // Net revenue after deductions
    int totalBookedSeats;              // Counter for the total number of booked seats
    int totalCanceledSeats;            // Counter for the total number of canceled seats
};

// Structure to store user details
struct user {
    char username[USERNAME_LENGTH];     // Username of the user
    char password[PASSWORD_LENGTH];     // User password
    char email[EMAIL_LENGTH];           // User email address
    char phone[PHONE_LENGTH];           // User phone number
    char address[ADDRESS_LENGTH];       // User address
};

// Array to store user details, with a maximum of MAX_USERS
struct user users[MAX_USERS];
int userCount = 1; // Starts with 1 since admin is the first user

// Admin user details (Default admin account)
struct user admin = {
    "Maneet",          // Admin username
    "1234",            // Admin password
    "maneet@gmail.com", // Admin email
    "1234567890",      // Admin phone number
    "Admin Address"    // Admin address
};

// Structure to store notification details
struct notification {
    union {
        char email[MAX_LENGTH];  // Email address for email notifications
        char phone[PHONE_LENGTH]; // Phone number for SMS notifications
    } recipient;                  // Union to store either email or phone

    char type[10];        // Notification type: "email" or "sms"
    char category[20];    // Notification category: "Confirmation", "Update", "Cancellation"
    int isEmail;          // Boolean flag: 1 for email, 0 for SMS
};

// --- User Management ---
void loadUsers();            // Load user data from a file
void saveUsers();            // Save user data to a file
int registerUser();          // Register a new user
int updateUser();            // Update user details
struct user loginUser();     // Handle user login
int loginAdmin();            // Handle admin login

// --- Bus Schedule Management ---
void addBusSchedule(struct BusReservation buses[], int *busCount, int maxBuses);  // Add a new bus schedule
void logBusUpdate(int busID, int ticketNumber, const char *oldValue, const char *newValue); // Log changes to bus schedule
void updateBusSchedule(struct BusReservation buses[], int *busCount, struct user currentUser); // Update an existing bus schedule
void deleteBusSchedule(struct BusReservation buses[], int *busCount); // Delete a bus schedule

// --- Bus Information Display ---
void printBus(struct BusReservation bus); // Print bus details
void checkBusStatus(struct BusReservation buses[], int busCount); // Check status of buses
void searchBuses(struct BusReservation buses[], int busCount); // Search for available buses
int loadBuses(struct BusReservation buses[], int maxBuses); // Load bus schedules from file
void saveBuses(struct BusReservation buses[], int busCount); // Save bus schedules to file

// --- Seat Management ---
int loadSeats(struct BusReservation buses[], int maxBuses); // Load seat reservation details
void saveSeats(struct BusReservation buses[], int busCount); // Save seat reservation details
void showSeats(struct BusReservation bus); // Display available and reserved seats
void viewAvailability(struct BusReservation buses[], int busCount); // View seat availability

// --- Booking Management ---
float calculateFare(int numSeats, float farePerSeat); // Calculate fare including taxes
int bookSeat(struct user currentUser, struct BusReservation *bus, int seatNumbers[], int *numSeats); // Book seats
void processBooking(struct user currentUser, struct BusReservation buses[], int *busCount); // Process the booking
void processPayment(float totalFare); // Handle payment process
void viewBookingHistory(struct user currentUser); // View user’s past bookings
void saveFrequentBooking(struct user currentUser, int busID, char *busNumberPlate, char *bookingDate, char *source, char *destination); // Save frequent bookings for quick access
int findFrequentBookings(struct user currentUser, char busNumberPlates[][20],char sources[][50], char destinations[][50], int *tripCount); // Find user’s frequent bookings
void bookFrequentBooking(struct user currentUser, struct BusReservation buses[], int busCount); // Book using frequent booking data
void finalizeBooking(struct user currentUser, struct BusReservation buses[], int busCount,int busIndex, int numSeats, int seatNumbers[], char *bookingDate,int tripIndex, int totalTrips, int *ticketNumbers, float *totalFares, int *busIndices); // Finalize booking process

// --- Ticket and Reservation Management ---
int generateTicketNumber(); // Generate a unique 6-digit ticket number
bool IsUnique(int ticketNumber); // Check if the ticket number is unique
int loadTicketNumbers(struct BusReservation buses[], int busCount); // Load ticket numbers from file
void saveReservation(struct user currentUser, int ticketNumber, int busID, char *busNumberPlate, int numSeats, int seatNumbers[], char *bookingDate, float finalAmount); // Save a reservation
void getTicketDetails(struct BusReservation buses[], int busCount); // Retrieve ticket details
void displayTicketDetails(struct BusReservation *bus, int ticketNumber, char *bookingDate); // Display a specific ticket's details

// --- Cancellation and Refund Management ---
void processRefund(float refundAmount); // Process refund after cancellation
void logCancellation(char *username, int ticketNumber, int busID, char *busNumberPlate, char *date, int numSeats, int canceledSeats[], float refundAmount); // Log a cancellation event
void updateFilesAfterCancellation(struct BusReservation buses[], int busCount, int busID, int numSeats, int ticketNumber, char *username); // Update files after cancellation
void cancelBooking(struct user currentUser, struct BusReservation buses[], int busCount); // Handle booking cancellation

// --- User Notifications ---
struct user getUserDetails(const char *username); // Retrieve user details
void saveNotification(struct notification *notif, int ticketNumber); // Save notifications to file
void checkAndRemoveUserUpdate(const char *currentUser); // Remove outdated user updates
void appendUserToTempFile(const char *username); // Append user data to a temporary file
void notifyUser(const char *username, int ticketNumber); // Notify user about ticket updates
void notifyUsersOfBusUpdate(int busID, const char *oldValue, const char *newValue); // Notify users of schedule changes
void viewNotifications(struct user *currentUser); // View all notifications
void displayNotification(const char *filename, const char *type, struct user *currentUser); // Display specific notifications
void printEmailMessage(const char *category, const char *recipient, int ticketNumber); // Print email notification
void printSmsMessage(const char *category, const char *recipient, int ticketNumber); // Print SMS notification

// --- Reports and Analytics ---
void generateReports(struct BusReservation buses[], int busCount); // Generate reports
void generateBusReport(struct BusReservation buses[], int busCount); // Generate report for buses
void printReportHeader(); // Print the header for a report
void printBusReport(); // Print bus reports
void filterBusReport(int filterType, char *filterValue, char comparison, float filterNumber); // Filter bus reports based on criteria
void generateUserReport(); // Generate report for user activities
void printHeader(const char *title); // Print reservation/cancellation report
void printUserReport(); // Print user reports
void printReservations(); // Print all reservations
void printCancellations(); // Print all canceled bookings
void filterRecords(const char *filename, int filterType, const char *filterValue); // Filter reservation/cancellation reports based on criteria
void viewReport(); // View generated reports


// Function to load users from the file into the users array
void loadUsers() {
    FILE *file = fopen("user.txt", "r");  // Open the file in read mode
    if (file == NULL) {  // Check if the file exists and can be opened
        printf("No user data found!\n");  // If the file does not exist, print an error message
        return;  // Exit the function
    }

    userCount = 0;  // Reset user count before loading to prevent duplicates

    // Read user details from the file and store them in the users array
    while (fscanf(file, "%s %s %s %s %[^\n]", // Read 5 space-separated values
                  users[userCount].username,  // Read username
                  users[userCount].password,  // Read password
                  users[userCount].email,     // Read email
                  users[userCount].phone,     // Read phone number
                  users[userCount].address) == 5) { // Read full address (allows spaces)
        userCount++;  // Increment the user count after successfully reading a user entry
    }

    fclose(file);  // Close the file after reading to free resources
}

// Function to save all user data to the file
void saveUsers() {
    FILE *file = fopen("user.txt", "w");  // Open the file in write mode (overwrite)
    if (file == NULL) {  // Check if file opening was successful
        printf("Error opening file for saving users.\n");
        return;
    }

    // Loop through all users in the users array and write their details to the file
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s %s %s %s %s\n",
                users[i].username,  // Write username
                users[i].password,  // Write password
                users[i].email,     // Write email
                users[i].phone,     // Write phone number
                users[i].address);  // Write address (includes spaces)
    }

    fclose(file);  // Close the file after writing to save changes
}

// Function to register a new user
int registerUser() {
    if (userCount >= MAX_USERS) {  // Check if the maximum number of users has been reached
        printf("User limit reached!\n");
        return 0;  // Exit registration process
    }

    struct user newUser;  // Create a temporary user structure to store input

    // Prompt the user to enter their details
    printf("Enter username: ");
    scanf("%s", newUser.username);
    printf("Enter password: ");
    scanf("%s", newUser.password);
    printf("Enter email: ");
    scanf("%s", newUser.email);
    printf("Enter phone number: ");
    scanf("%s", newUser.phone);
    printf("Enter address: ");
    scanf(" %[^\n]", newUser.address);  // %[^\n] allows spaces in the address input

    // Check if the username already exists in the system
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, newUser.username) == 0) {  // Compare new username with existing ones
            printf("Username already taken. Try another one.\n");
            return 0;  // Exit function without registering the user
        }
    }

    users[userCount++] = newUser;  // Add new user to the users array and increment userCount

    saveUsers();  // Save the updated user list to the file

    printf("Registration successful!\n");  // Confirm successful registration
    return 1;  // Return success
}

// Function to update an existing user's details
int updateUser() {
    char username[50];  // Store the username entered by the user
    printf("Enter your username: ");
    scanf("%s", username);  // Read username input

    int found = -1;  // Variable to store index of found user (-1 means not found)

    // Search for the user in the users array
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {  // Compare input username with stored usernames
            found = i;  // Store the index of the found user
            break;  // Exit loop once the user is found
        }
    }

    if (found == -1) {  // If no matching username was found
        printf("User not found!\n");
        return 0;  // Exit the function
    }

    int choice;  // Store user menu choice
    do {
        // Display update options
        printf("\nWhat would you like to update?\n");
        printf("1. Password\n");
        printf("2. Email\n");
        printf("3. Phone Number\n");
        printf("4. Address\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);  // Read the user's menu choice

        switch (choice) {
            case 1:
                printf("Enter new password: ");
                scanf("%s", users[found].password);  // Update password
                break;
            case 2:
                printf("Enter new email: ");
                scanf("%s", users[found].email);  // Update email
                break;
            case 3:
                printf("Enter new phone number: ");
                scanf("%s", users[found].phone);  // Update phone number
                break;
            case 4:
                printf("Enter new address: ");
                scanf(" %[^\n]", users[found].address);  // Update address (allows spaces)
                break;
            case 5:
                printf("Exiting update menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }

        // Save changes to file if a valid update was made
        if (choice >= 1 && choice <= 4) {
            FILE *file = fopen("user.txt", "w");  // Open file in write mode to save updates
            if (file == NULL) {  // Check if file opening failed
                printf("Error saving user data!\n");
                return 0;
            }

            // Save all users (including updated user) back to the file
            for (int i = 0; i < userCount; i++) {
                fprintf(file, "%s %s %s %s %s\n",
                        users[i].username,  // Write username
                        users[i].password,  // Write password
                        users[i].email,     // Write email
                        users[i].phone,     // Write phone number
                        users[i].address);  // Write address
            }

            fclose(file);  // Close the file after writing
            printf("Information updated successfully!\n");  // Confirm success
        }

    } while (choice != 5);  // Repeat the menu until user chooses to exit

    return 1;  // Return success
}

// Function to log in a regular user
struct user loginUser() {
    struct user foundUser = { "", "", "", "", ""}; // Initialize user struct with empty values
    char inputUsername[USERNAME_LENGTH], inputPassword[PASSWORD_LENGTH]; // Variables for user input
    int attempts = 0, usernameValid = 0; // Track login attempts and username validation

    while (attempts < MAX_ATTEMPTS) { // Allow multiple login attempts
        if (!usernameValid) { // Ask for username only if not already validated
            printf("Enter Username: ");
            scanf("%49s", inputUsername); // Read username input (max length 49 to prevent overflow)
        }

        printf("Enter Password: ");
        scanf("%49s", inputPassword); // Read password input

        FILE *file = fopen("user.txt", "r"); // Open the user data file for reading
        if (!file) { // Check if file opening failed
            printf("Error opening user file!\n");
            exit(1); // Exit program if file is missing or inaccessible
        }

        usernameValid = 0; // Reset username validity flag before checking

        // Read user details from the file and compare with input credentials
        while (fscanf(file, "%s %s %s %s %[^\n]",
                      foundUser.username, foundUser.password,
                      foundUser.email, foundUser.phone, foundUser.address) == 5) {

            if (strcmp(inputUsername, foundUser.username) == 0) { // Check if username matches
                usernameValid = 1; // Mark username as found

                if (strcmp(inputPassword, foundUser.password) == 0) { // Check if password matches
                    fclose(file); // Close file before returning user data
                    printf("Login successful! Welcome, %s\n", foundUser.username);

                    checkAndRemoveUserUpdate(foundUser.username); // Process user updates if necessary
                    return foundUser; // Return the logged-in user data
                } else {
                    printf("Incorrect password! Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
                    break; // Allow user to retry password without re-entering username
                }
            }
        }

        fclose(file); // Close file after searching

        if (!usernameValid) { // If username was not found, prompt again
            printf("Username not found! Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
        }

        attempts++; // Increment login attempt count
    }

    printf("Too many failed attempts. Exiting...\n");
    exit(1); // Exit program after too many failed attempts
}

// Function to log in an admin user
int loginAdmin() {
    char username[USERNAME_LENGTH], password[PASSWORD_LENGTH]; // Variables for admin input
    int attempts = 0, usernameValid = 0; // Track login attempts and username validation

    while (attempts < MAX_ATTEMPTS) { // Allow multiple login attempts
        if (!usernameValid) { // Ask for username only if not validated yet
            printf("Enter Admin Username: ");
            scanf("%49s", username); // Read admin username
        }

        printf("Enter Admin Password: ");
        scanf("%49s", password); // Read admin password

        if (strcmp(username, admin.username) == 0) { // Check if username matches stored admin credentials
            usernameValid = 1; // Mark username as found

            if (strcmp(password, admin.password) == 0) { // Check if password matches
                printf("Admin login successful! Welcome, Admin!\n");
                return 1; // Return success
            } else {
                printf("Incorrect password! Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
            }
        } else {
            printf("Admin username not found! Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
            usernameValid = 0; // Reset validity check to re-enter username
        }

        attempts++; // Increment login attempt count
    }

    printf("Too many failed attempts. Exiting...\n");
    exit(1); // Exit program after exceeding max attempts
}

// Function to add a new bus schedule
void addBusSchedule(struct BusReservation buses[], int *busCount, int maxBuses) {
    // Check if adding another bus exceeds the maximum allowed limit
    if (*busCount >= maxBuses) {
        printf("Cannot add more buses. Maximum limit reached.\n");
        return;
    }

    // Pointer to the next available slot in the bus list
    struct BusReservation *bus = &buses[*busCount];

    // Collecting bus information from the user
    printf("\nEnter Bus ID: ");
    scanf("%d", &bus->busID); // Unique ID for the bus

    printf("Enter Bus Number Plate: ");
    scanf("%19s", bus->busNumberPlate); // License plate of the bus

    printf("Enter Date (YYYY-MM-DD): ");
    scanf("%10s", bus->date); // Travel date in a standard format

    printf("Enter Source: ");
    scanf("%49s", bus->source); // City or location where the journey starts

    printf("Enter Destination: ");
    scanf("%49s", bus->destination); // Final destination of the bus

    printf("Enter Departure Time: ");
    scanf("%19s", bus->departureTime); // Time when the bus departs

    printf("Enter Arrival Time: ");
    scanf("%19s", bus->arrivalTime); // Expected arrival time at the destination

    printf("Enter Total Seats: ");
    scanf("%d", &bus->totalSeats); // Total seating capacity of the bus

    bus->availableSeats = bus->totalSeats; // Initially, all seats are unoccupied

    printf("Enter Fare (RM): ");
    scanf("%f", &bus->fare); // Cost of a single ticket for the bus

    bus->reservedCount = 0; // No reservations initially

    (*busCount)++; // Increment the count of registered buses

    saveBuses(buses, *busCount); // Save updated bus data to the file

    printf("Bus schedule added successfully!\n");
}

// Function to log changes made to a bus schedule
void logBusUpdate(int busID, int ticketNumber, const char *oldValue, const char *newValue) {
    // Open the file where updates are logged, using append mode
    FILE *file = fopen("updates.txt", "a");
    if (!file) {
        printf("Error: Could not open updates.txt for writing.\n");
        return;
    }

    // Write the update details in a structured format
    fprintf(file, "%d, %d, %s, %s\n", ticketNumber, busID, oldValue, newValue);
    fclose(file); // Close the file to save changes
}

// Function to modify an existing bus schedule
void updateBusSchedule(struct BusReservation buses[], int *busCount, struct user currentUser) {
    int busID, choice, found = 0;

    // Prompt user to enter the Bus ID they want to update
    printf("\nEnter Bus ID to update: ");
    scanf("%d", &busID);

    // Search for the bus in the list by comparing Bus ID
    for (int i = 0; i < *busCount; i++) {
        if (buses[i].busID == busID) { // If the entered Bus ID matches a stored bus
            found = 1; // Mark that we found the bus

            // Display update options to the user
            printf("\nBus found! What would you like to update?\n");
            printf("1. Date\n2. Bus Number Plate\n3. Source\n4. Destination\n5. Departure Time\n6. Arrival Time\n7. Total Seats\n8. Fare\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            // Variables to store old and new values for logging
            char oldValue[50], newValue[50];

            // Switch case to handle each update option
            switch (choice) {
                case 1: // Update travel date
                    strcpy(oldValue, buses[i].date); // Store old date for logging
                    printf("Enter new Date (YYYY-MM-DD): ");
                    scanf("%10s", buses[i].date); // Get new date input
                    strcpy(newValue, buses[i].date); // Store new date for logging
                    break;

                case 2: // Update bus number plate
                    strcpy(oldValue, buses[i].busNumberPlate); // Store old plate number
                    printf("Enter new Bus Number Plate: ");
                    scanf("%19s", buses[i].busNumberPlate); // Get new plate number
                    strcpy(newValue, buses[i].busNumberPlate); // Store new plate for logging
                    break;

                case 3: // Update bus source location
                    strcpy(oldValue, buses[i].source); // Store old source
                    printf("Enter new Source: ");
                    scanf("%49s", buses[i].source); // Get new source input
                    strcpy(newValue, buses[i].source); // Store new source for logging
                    break;

                case 4: // Update bus destination location
                    strcpy(oldValue, buses[i].destination); // Store old destination
                    printf("Enter new Destination: ");
                    scanf("%49s", buses[i].destination); // Get new destination input
                    strcpy(newValue, buses[i].destination); // Store new destination for logging
                    break;

                case 5: // Update departure time
                    strcpy(oldValue, buses[i].departureTime); // Store old departure time
                    printf("Enter new Departure Time: ");
                    scanf("%19s", buses[i].departureTime); // Get new departure time input
                    strcpy(newValue, buses[i].departureTime); // Store new time for logging
                    break;

                case 6: // Update arrival time
                    strcpy(oldValue, buses[i].arrivalTime); // Store old arrival time
                    printf("Enter new Arrival Time: ");
                    scanf("%19s", buses[i].arrivalTime); // Get new arrival time input
                    strcpy(newValue, buses[i].arrivalTime); // Store new time for logging
                    break;

                case 7: { // Update total seats
                    int bookedSeats = buses[i].totalSeats - buses[i].availableSeats; // Calculate already booked seats
                    int newTotalSeats;
                    sprintf(oldValue, "%d", buses[i].totalSeats); // Store old seat count
                    printf("Enter new Total Seats: ");
                    scanf("%d", &newTotalSeats); // Get new seat count input

                    // Ensure new total seats are not less than already booked seats
                    if (newTotalSeats < bookedSeats) {
                        printf("Error: New total seats cannot be less than already booked seats (%d).\n", bookedSeats);
                        return; // Exit function if invalid input
                    }

                    buses[i].totalSeats = newTotalSeats; // Update total seats
                    buses[i].availableSeats = newTotalSeats - bookedSeats; // Adjust available seats
                    sprintf(newValue, "%d", newTotalSeats); // Store new seat count for logging
                    break;
                }

                case 8: // Update bus fare
                    sprintf(oldValue, "%.2f", buses[i].fare); // Store old fare value
                    printf("Enter new Fare (RM): ");
                    scanf("%f", &buses[i].fare); // Get new fare input
                    sprintf(newValue, "%.2f", buses[i].fare); // Store new fare for logging
                    break;

                default: // Handle invalid choice input
                    printf("Invalid choice!\n");
                    return;
            }

            // Save the updated bus details to file
            saveBuses(buses, *busCount);
            printf("Bus schedule updated successfully!\n");

            // Notify users about the schedule update if needed
            notifyUsersOfBusUpdate(busID, oldValue, newValue);
            return;
        }
    }

    // If no bus with the entered Bus ID is found, display an error message
    if (!found) {
        printf("Bus ID not found!\n");
    }
}

// Function to remove a bus from the schedule
void deleteBusSchedule(struct BusReservation buses[], int *busCount) {
    int busID, found = 0; // Variables to store the bus ID and a flag to check if the bus is found

    // Prompt user to enter the Bus ID that needs to be deleted
    printf("\nEnter Bus ID to delete: ");
    scanf("%d", &busID);

    // Loop through the list of buses to find the matching Bus ID
    for (int i = 0; i < *busCount; i++) {
        if (buses[i].busID == busID) { // If the entered Bus ID matches a stored bus
            found = 1; // Set flag indicating the bus was found

            // Shift all subsequent buses left to overwrite the deleted bus entry
            for (int j = i; j < *busCount - 1; j++) {
                buses[j] = buses[j + 1]; // Move the next bus one position up
            }

            // Reduce the total bus count since one bus is deleted
            (*busCount)--;

            printf("Bus schedule deleted successfully.\n");
            break; // Exit the loop since deletion is completed
        }
    }

    // If the bus with the entered Bus ID was not found, display an error message
    if (!found) {
        printf("Bus ID not found!\n");
    }

    // Save the updated bus list to file after deletion
    saveBuses(buses, *busCount);
}

// Function to print a single bus's details in a formatted table row.
void printBus(struct BusReservation bus) {
    printf("| %-5d | %-12s | %-10s | %-15s | %-15s | %-10s | %-10s | %-5d | %-5d | %-6.2f |\n",
           bus.busID, bus.busNumberPlate, bus.date, bus.source, bus.destination,
           bus.departureTime, bus.arrivalTime, bus.totalSeats, bus.availableSeats, bus.fare);
}

// Function to display the status of all buses in the system.
void checkBusStatus(struct BusReservation buses[], int busCount) {
    // If no buses exist, inform the user and return.
    if (busCount == 0) {
        printf("\nNo buses found in the system!\n");
        return;
    }

    // Print the header for the table.
    printBusHeader();

    // Iterate through the list of buses and print each one's details.
    for (int i = 0; i < busCount; i++) {
        printBus(buses[i]);
    }

    // Print a line to mark the end of the bus list.
    printf("============================================================================================================================\n");
}

// Function to search for available buses based on user input.
void searchBuses(struct BusReservation buses[], int busCount) {
    char source[50], destination[50]; // Stores the source and destination entered by the user.
    int tripType; // Determines whether the user wants a one-way or round-trip search.

    // Ask user if they want a one-way or round trip.
    printf("Enter 1 for One-way trip, 2 for Return trip: ");
    scanf("%d", &tripType);
    getchar(); // Consume the newline character left in input buffer.

    // Prompt for source location.
    printf("Enter Source: ");
    fgets(source, sizeof(source), stdin);
    source[strcspn(source, "\n")] = '\0'; // Remove trailing newline.

    // Prompt for destination location.
    printf("Enter Destination: ");
    fgets(destination, sizeof(destination), stdin);
    destination[strcspn(destination, "\n")] = '\0'; // Remove trailing newline.

    int found = 0; // Tracks whether a matching bus is found.

    // Search for one-way trip buses.
    printf("\n--- One-way trip buses ---\n");
    printBusHeader();
    for (int i = 0; i < busCount; i++) {
        // Compare the source and destination case-insensitively.
        if (strcasecmp(buses[i].source, source) == 0 && strcasecmp(buses[i].destination, destination) == 0) {
            printBus(buses[i]); // Print matching bus details.
            found = 1; // Mark that at least one bus was found.
        }
    }
    printf("============================================================================================================================\n");

    // If user requested a round trip, search for return route as well.
    if (tripType == 2) {
        printf("\n--- Return trip buses ---\n");
        printBusHeader();
        for (int i = 0; i < busCount; i++) {
            // Look for buses where the source and destination are swapped.
            if (strcasecmp(buses[i].source, destination) == 0 && strcasecmp(buses[i].destination, source) == 0) {
                printBus(buses[i]);
                found = 1;
            }
        }
        printf("============================================================================================================================\n");
    }

    // If no matching buses were found, inform the user.
    if (!found) {
        printf("\nNo matching buses found for the given route(s).\n");
    }
}

// Function to load bus schedules from a file into memory.
int loadBuses(struct BusReservation buses[], int maxBuses) {
    FILE *file = fopen("buses.txt", "r"); // Open the file in read mode.
    if (!file) { // Check if the file was opened successfully.
        printf("Error: Could not open buses.txt for reading.\n");
        return 0; // Return 0 to indicate that no buses were loaded.
    }

    int count = 0; // Tracks the number of buses successfully loaded.

    // Read bus details from the file while respecting the max bus limit.
    while (count < maxBuses && fscanf(file, "%d,%19[^,],%10[^,],%49[^,],%49[^,],%19[^,],%19[^,],%d,%d,%f",
                  &buses[count].busID, buses[count].busNumberPlate, buses[count].date,
                  buses[count].source, buses[count].destination,
                  buses[count].departureTime, buses[count].arrivalTime,
                  &buses[count].totalSeats, &buses[count].availableSeats,
                  &buses[count].fare) == 10) { // Ensure all 10 fields are read successfully.
        buses[count].reservedCount = 0; // Initialize reserved seat count to 0.
        count++; // Increment the counter after successfully reading a bus record.
    }

    fclose(file); // Close the file after reading.
    return count; // Return the number of buses loaded.
}

void saveBuses(struct BusReservation buses[], int busCount) {
    FILE *file = fopen("buses.txt", "w"); // Open the file in write mode
    if (!file) {
        printf("Error: Could not open buses.txt for writing.\n");
        return; // Exit the function if the file cannot be opened
    }

    // Iterate through the list of buses and write their details to the file
    for (int i = 0; i < busCount; i++) {
        fprintf(file, "%d,%s,%s,%s,%s,%s,%s,%d,%d,%.2f\n",
                buses[i].busID, buses[i].busNumberPlate, buses[i].date,
                buses[i].source, buses[i].destination,
                buses[i].departureTime, buses[i].arrivalTime,
                buses[i].totalSeats, buses[i].availableSeats, buses[i].fare);
    }

    fclose(file); // Close the file after writing
}

int loadSeats(struct BusReservation buses[], int maxBuses) {
    FILE *file = fopen("seats.txt", "r"); // Open the seats file in read mode
    if (!file) return 0; // Return 0 if file cannot be opened

    int count = 0; // Tracks number of buses loaded

    // Read bus seat data from the file while space is available in array
    while (fscanf(file, "%d,%d", &buses[count].busID, &buses[count].reservedCount) == 2) {
        // Read the reserved seat numbers for the bus
        for (int i = 0; i < buses[count].reservedCount; i++) {
            fscanf(file, ",%d", &buses[count].reservedSeats[i]);
        }
        count++; // Increment the count of buses loaded

        // Stop reading if we reach the maximum allowed buses
        if (count >= maxBuses) break;
    }

    fclose(file); // Close the file after reading
    return count; // Return the number of buses loaded from file
}

void saveSeats(struct BusReservation buses[], int busCount) {
    FILE *file = fopen("seats.txt", "w"); // Open the file in write mode
    if (!file) {
        printf("Error opening seats.txt for writing!\n");
        return; // Exit the function if the file cannot be opened
    }

    // Iterate through each bus and save its seat reservation details
    for (int i = 0; i < busCount; i++) {
        fprintf(file, "%d,%d", buses[i].busID, buses[i].reservedCount); // Write bus ID and reserved count

        // Write each reserved seat number to the file
        for (int j = 0; j < buses[i].reservedCount; j++) {
            fprintf(file, ",%d", buses[i].reservedSeats[j]);
        }
        fprintf(file, "\n"); // Move to the next line for the next bus
    }

    fclose(file); // Close the file after writing
}

void showSeats(struct BusReservation bus) {
    int reservedSeats[MAX_SEATS] = {0}; // Initialize an array to track reserved seats

    // Mark the reserved seats in the array
    for (int i = 0; i < bus.reservedCount; i++) {
        reservedSeats[bus.reservedSeats[i] - 1] = 1; // Mark the seat as reserved
    }

    printf("\nSeats Layout (Available in Green, Reserved in Red):\n");

    // Display seat numbers with color coding for availability
    for (int i = 0; i < bus.totalSeats; i++) {
        if (reservedSeats[i]) {
            printf("\033[0;31m[%2d]\033[0m ", i + 1); // Reserved seats in red
        } else {
            printf("\033[0;32m[%2d]\033[0m ", i + 1); // Available seats in green
        }

        // Extra spacing to visually separate seat sections
        if ((i + 1) % (SEATS_PER_ROW / 2) == 0) {
            printf("   "); // Adds space between left and right seat sections
        }

        // Move to the next line after every SEATS_PER_ROW seats
        if ((i + 1) % SEATS_PER_ROW == 0) {
            printf("\n");
        }
    }
    printf("\n"); // Ensure there's a newline at the end for readability
}

void viewAvailability(struct BusReservation buses[], int busCount) {
    int busID, busIndex = -1; // Initialize variables
    printf("Enter Bus ID to check availability: ");
    scanf("%d", &busID);

    // Search for the bus with the given ID
    for (int i = 0; i < busCount; i++) {
        if (buses[i].busID == busID) {
            busIndex = i; // Store the index of the matching bus
            break;
        }
    }

    if (busIndex == -1) {
        printf("Error: Bus ID not found!\n"); // Notify user if bus doesn't exist
    } else {
        showSeats(buses[busIndex]); // Display seat availability for the found bus
    }
}

float calculateFare(int numSeats, float farePerSeat) {
    float tripFare = numSeats * farePerSeat; // Calculate the total base fare for the seats.
    float sst = tripFare * SST_RATE;        // Compute SST (Sales & Services Tax).
    float finalAmount = tripFare + sst;     // Add SST to the base fare to get the final price.

    return finalAmount; // Return the total fare amount including tax.
}

int bookSeat(struct user currentUser, struct BusReservation *bus, int seatNumbers[], int *numSeats) {
    printf("\nHow many seats to book? ");

    // Get user input for the number of seats and validate it.
    if (scanf("%d", numSeats) != 1 || *numSeats <= 0) {
        printf("Error: Invalid seat count!\n");
        return 0; // Exit if invalid input is provided.
    }

    // Check if there are enough available seats for the booking.
    if (*numSeats > bus->availableSeats) {
        printf("Error: Not enough available seats!\n");
        return 0; // Exit if not enough seats are available.
    }

    printf("Enter seat numbers: ");
    for (int i = 0; i < *numSeats; i++) {
        scanf("%d", &seatNumbers[i]);  // Store the seat number in the array.

        // Validate seat range to ensure it's within the allowed range.
        if (seatNumbers[i] < 1 || seatNumbers[i] > MAX_SEATS) {
            printf("Error: Seat number %d is out of range! Try again.\n", seatNumbers[i]);
            return 0; // Exit if an invalid seat number is entered.
        }

        // Check if the selected seat is already reserved.
        for (int j = 0; j < bus->reservedCount; j++) {
            if (bus->reservedSeats[j] == seatNumbers[i]) {
                printf("Error: Seat %d is already booked! Try again.\n", seatNumbers[i]);
                return 0; // Exit if the seat is already taken.
            }
        }
    }

    // Generate a unique ticket number for this booking.
    int ticketNumber = generateTicketNumber();

    // Store the booked seat numbers in the bus reservation record.
    for (int j = 0; j < *numSeats; j++) {
        bus->reservedSeats[bus->reservedCount++] = seatNumbers[j];  // Mark seat as reserved.
    }

    // Save the ticket number and seat count for this booking.
    bus->ticketNumbers[bus->bookingCount] = ticketNumber;
    bus->seatCounts[bus->bookingCount] = *numSeats;
    bus->bookingCount++;

    // Update the available seats count after booking.
    bus->availableSeats -= *numSeats;

    return ticketNumber;  // Return the generated ticket number.
}

void processBooking(struct user currentUser, struct BusReservation buses[], int *busCount) {
    int tripType, busID, busIndex;
    char bookingDate[20];

    // Prompt the user to select the type of trip they want to book.
    printf("\n1. One-Way\n2. Round Trip\n3. Frequent Booking\n4. Back to Main Menu\nEnter choice: ");

    // Validate user input to ensure a correct selection
    if (scanf("%d", &tripType) != 1 || tripType < 1 || tripType > 4) {
        printf("Error: Invalid choice!\n");
        return; // Exit if input is invalid
    }

    // If the user selects option 4 (Back to Main Menu), exit the function.
    if (tripType == 4) return;

    // If the user selects "Frequent Booking," handle that scenario separately.
    if (tripType == 3) {
        bookFrequentBooking(currentUser, buses, *busCount);
        return;
    }

    // Generate the current date in YYYY-MM-DD format for the booking.
    time_t t;
    time(&t);
    strftime(bookingDate, sizeof(bookingDate), "%Y-%m-%d", localtime(&t));

    // Arrays to store booking details for up to 2 trips (for round-trip bookings).
    int ticketNumbers[2];  // Stores ticket numbers for one-way or round-trip.
    float totalFares[2];   // Stores fare details for each booked trip.
    int busIndices[2];     // Stores indices of the selected buses.

    // Loop runs once for one-way, twice for round-trip bookings.
    for (int tripIndex = 0; tripIndex < (tripType == 2 ? 2 : 1); tripIndex++) {
        printf("\nEnter Bus ID for %s: ", (tripIndex == 0) ? "One-Way" : "Return Trip");

        // Validate user input for Bus ID
        if (scanf("%d", &busID) != 1) {
            printf("Error: Invalid input!\n");
            return; // Exit function if input is invalid
        }

        // Search for the bus with the given Bus ID in the available buses array.
        busIndex = -1; // Initialize as -1 (not found)
        for (int j = 0; j < *busCount; j++) {
            if (buses[j].busID == busID) {
                busIndex = j; // Store index if bus is found
                break;
            }
        }

        // If no matching bus is found, display an error and allow retrying.
        if (busIndex == -1) {
            printf("Error: No matching bus found for the given Bus ID!\n");
            continue; // Skip to the next iteration, allowing the user to retry
        }

        // Store the bus index for finalizing the booking later.
        busIndices[tripIndex] = busIndex;

        // Display available seats for the selected bus.
        showSeats(buses[busIndex]);

        // Attempt to book seats for the selected bus.
        int seatNumbers[MAX_SEATS], numSeats;
        if (bookSeat(currentUser, &buses[busIndex], seatNumbers, &numSeats) > 0) {
            // If seat booking is successful, finalize the booking process.
            finalizeBooking(currentUser, buses, *busCount, busIndex, numSeats, seatNumbers,bookingDate, tripIndex, tripType, ticketNumbers, totalFares, busIndices);
        } else {
            // If no seats were successfully booked, allow the user to retry.
            printf("No seats booked. Please try again.\n");
            tripIndex--;  // Decrease the index to retry booking for the same trip.
        }
    }
}

void processPayment(float totalFare) {
    int paymentMethod, walletChoice;
    int valid = 0; // Flag to check if payment was successful

    do {
        // Display available payment options
        printf("\nSelect Payment Method:\n");
        printf("1. Credit/Debit Card\n2. Digital Wallet\n3. Cancel Payment\nEnter choice: ");
        scanf("%d", &paymentMethod);

        switch (paymentMethod) {
            case 1: { // Credit/Debit Card Payment
                char cardNumber[20]; // To store card number input
                printf("Enter card number (16 digits): ");
                scanf("%s", cardNumber);

                // Validate that the entered card number has exactly 16 digits
                if (strlen(cardNumber) != 16) {
                    printf("Invalid card number! Please try again.\n");
                    continue; // Ask for input again
                }

                valid = 1; // Mark payment as successful
                printf("Payment of RM %.2f completed via Credit/Debit Card.\n", totalFare);
                break;
            }
            case 2: { // Digital Wallet Payment
                printf("\nSelect a Digital Wallet:\n");
                printf("1. Touch 'n Go eWallet\n");
                printf("2. GrabPay\n");
                printf("3. ShopeePay\n");
                printf("Enter choice: ");
                scanf("%d", &walletChoice);

                // Validate the selected wallet option
                if (walletChoice < 1 || walletChoice > 3) {
                    printf("Invalid Wallet Selection! Try Again.\n");
                    continue; // Restart the loop
                }

                // Simulating processing of payment
                printf("Processing Digital Wallet payment...\n");
                valid = 1; // Mark payment as successful
                printf("Payment of RM %.2f completed via ", totalFare);

                // Display payment confirmation based on selected wallet
                switch (walletChoice) {
                    case 1: printf("Touch 'n Go eWallet.\n"); break;
                    case 2: printf("GrabPay.\n"); break;
                    case 3: printf("ShopeePay.\n"); break;
                }
                break;
            }
            case 3: // Cancel Payment Option
                printf("Payment canceled.\n");
                return; // Exit the function
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (!valid); // Repeat until payment is successful

    // Display payment success message and generate a receipt
    printf("\nPayment Successful\n");
    printf("Generating receipt...\n");
    printf("=================================\n");
    printf(" Total Paid: RM %.2f\n", totalFare);
    printf(" Payment Method: %s\n",
           (paymentMethod == 1) ? "Credit/Debit Card" :
           (walletChoice == 1) ? "Touch 'n Go eWallet" :
           (walletChoice == 2) ? "GrabPay" : "ShopeePay");
    printf(" Thank you for your booking!\n");
    printf("=================================\n");
}

void viewBookingHistory(struct user currentUser) {
    FILE *file = fopen("reservation.txt", "r"); // Open reservation file in read mode
    if (!file) {
        printf("Error: Could not open reservation file!\n");
        return; // Exit function if file is not found
    }

    char username[50];         // Stores username from file
    int ticketNumber, busID, numSeats;
    char busPlateNumber[20], date[20]; // Stores bus details
    int seatNumbers[10];       // Assumption: Max 10 seats per booking
    float finalAmount;
    int found = 0;             // Flag to track if user has any bookings

    // Display header for booking history
    printf("\n===========================================\n");
    printf("         Booking History - %s\n", currentUser.username);
    printf("===========================================\n");

    // Read each booking entry from file
    while (fscanf(file, "%49[^,],%d,%d,%19[^,],%19[^,],%d,",
                  username, &ticketNumber, &busID, busPlateNumber, date, &numSeats) == 6) {
        // Check if the record belongs to the current user
        if (strcmp(username, currentUser.username) == 0) {
            found = 1; // Mark that at least one booking was found

            // Read seat numbers for the booking
            for (int i = 0; i < numSeats; i++) {
                fscanf(file, "%d,", &seatNumbers[i]);
            }
            fscanf(file, "%f\n", &finalAmount); // Read final amount paid

            // Display booking details
            printf("\nBus ID: %d\n", busID);
            printf("Bus Plate: %s\n", busPlateNumber);
            printf("Travel Date: %s\n", date);
            printf("Ticket No: %d\n", ticketNumber);
            printf("Seats: ");
            for (int i = 0; i < numSeats; i++) {
                printf("%d ", seatNumbers[i]); // Print seat numbers
            }
            printf("\nTotal Amount: RM %.2f\n", finalAmount);
            printf("-------------------------------------------\n");
        } else {
            fscanf(file, "%*[^\n]\n"); // Skip the remaining data of unmatched records
        }
    }

    fclose(file); // Close file after reading

    if (!found) { // If no bookings were found
        printf("\nNo booking history found.\n");
    }

    printf("\n===========================================\n");
}

void saveFrequentBooking(struct user currentUser, int busID, char *busNumberPlate, char *bookingDate, char *source, char *destination) {
    FILE *file = fopen("reservation.txt", "r");
    if (!file) {
        printf("Error: Could not open reservation file!\n");
        return;
    }

    char line[300], username[50], busPlate[20], date[15];
    int ticketNumber, resBusID, numSeats, count = 0;
    float fare;

    // Count how many times this user booked the same bus
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%d,%d,%[^,],%[^,],%d,", username, &ticketNumber, &resBusID, busPlate, date, &numSeats);
        if (strcmp(username, currentUser.username) == 0 && strcmp(busPlate, busNumberPlate) == 0) {
            count++;
        }
    }
    fclose(file);

    // If booked 5+ times, check if it's already saved
    if (count >= 5) {
        file = fopen("frequent_bookings.txt", "r");
        if (!file) {
            printf("Error: Could not open frequent bookings file for reading!\n");
            return;
        }

        int alreadySaved = 0;
        while (fgets(line, sizeof(line), file)) {
            char savedUser[50], savedBusPlate[20], savedSource[50], savedDestination[50];
            sscanf(line, "%[^,],%[^,],%[^,],%[^\n]", savedUser, savedBusPlate, savedSource, savedDestination);

            if (strcmp(savedUser, currentUser.username) == 0 && strcmp(savedBusPlate, busNumberPlate) == 0 &&
                strcmp(savedSource, source) == 0 && strcmp(savedDestination, destination) == 0) {
                alreadySaved = 1;
                break;
            }
        }
        fclose(file);

        // Save only if not already present
        if (!alreadySaved) {
            file = fopen("frequent_bookings.txt", "a");
            if (!file) {
                printf("Error: Could not open frequent bookings file for writing!\n");
                return;
            }
            fprintf(file, "%s,%s,%s,%s\n", currentUser.username, busNumberPlate, source, destination);
            fclose(file);
            printf("Frequent booking saved for %s: Bus %s (%s -> %s)!\n", currentUser.username, busNumberPlate, source, destination);
        }
    }
}

int findFrequentBookings(struct user currentUser, char busNumberPlates[][20],char sources[][50], char destinations[][50], int *tripCount) {
    FILE *file = fopen("frequent_bookings.txt", "r");
    if (!file) {
        printf("Error: Could not open frequent bookings file!\n");
        return 0;
    }

    char username[50], busPlate[20], src[50], dest[50];
    *tripCount = 0;

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^\n]\n", username, busPlate, src, dest) == 4) {
        if (strcmp(username, currentUser.username) == 0) {
            strcpy(busNumberPlates[*tripCount], busPlate);
            strcpy(sources[*tripCount], src);
            strcpy(destinations[*tripCount], dest);
            (*tripCount)++;  // Increase trip count
        }
    }

    fclose(file);
    return (*tripCount > 0);
}

void bookFrequentBooking(struct user currentUser, struct BusReservation buses[], int busCount) {
    // Declare necessary variables to hold booking details like travel dates and frequent trip details.
    char bookingDate[20], travelDate[20], returnTravelDate[20];  // Stores booking date, travel date, and return travel date.
    char frequentBusNumberPlates[2][20], sources[2][50], destinations[2][50];  // Stores frequent bus details: bus number plate, source, and destination.
    int tripCount = 0;  // Counter to track the number of frequent trips the user has.

    // Call the findFrequentBookings function to populate the frequent bus details
    // for the given user into the arrays 'frequentBusNumberPlates', 'sources', and 'destinations'.
    if (!findFrequentBookings(currentUser, frequentBusNumberPlates, sources, destinations, &tripCount)) {
        printf("You have no frequent bookings.\n");
        return;  // Exit if no frequent bookings found.
    }

    // Display the list of frequent bookings available for the user.
    printf("Frequent Bookings Found:\n");
    for (int i = 0; i < tripCount; i++) {
        // Print each frequent trip with its bus number plate, source, and destination.
        printf("%d. %s (%s → %s)\n", i + 1, frequentBusNumberPlates[i], sources[i], destinations[i]);
    }

    // Ask the user to select a frequent trip from the list of frequent bookings.
    int tripChoice;
    printf("Select a trip: ");
    // Validate input to ensure a valid selection is made (must be a number and within the range of available trips).
    if (scanf("%d", &tripChoice) != 1 || tripChoice < 1 || tripChoice > tripCount) {
        printf("Invalid selection.\n");
        return;  // Exit if the selection is invalid.
    }

    // Declare variables to store the details of the selected frequent trip.
    char selectedBusNumberPlate[20], selectedSource[50], selectedDestination[50];
    // Copy the selected bus number plate, source, and destination to the respective variables.
    strcpy(selectedBusNumberPlate, frequentBusNumberPlates[tripChoice - 1]);
    strcpy(selectedSource, sources[tripChoice - 1]);
    strcpy(selectedDestination, destinations[tripChoice - 1]);

    // Get the current date for booking.
    time_t t;
    time(&t);
    strftime(bookingDate, sizeof(bookingDate), "%Y-%m-%d", localtime(&t));  // Format date as YYYY-MM-DD.

    // Step 4: Display available dates for the selected trip
    printf("\nAvailable Dates for the trip:\n");
    // Declare an array to hold available travel dates for the selected route.
    char availableDates[MAX_BOOKINGS][11];  // Stores available travel dates.
    int dateCount = 0;  // Counter to track the number of available travel dates.

    // Iterate through all buses and find the ones that match the selected bus route (bus number, source, and destination).
    // For each matching bus, add the available travel date to the availableDates array.
    for (int i = 0; i < busCount; i++) {
        if (strcmp(buses[i].busNumberPlate, selectedBusNumberPlate) == 0 &&
            strcmp(buses[i].source, selectedSource) == 0 &&
            strcmp(buses[i].destination, selectedDestination) == 0) {
            strcpy(availableDates[dateCount], buses[i].date);  // Add the travel date to the array.
            dateCount++;  // Increment the count of available dates.
        }
    }

    // If no available dates are found, display a message and exit.
    if (dateCount == 0) {
        printf("No available dates for this frequent route.\n");
        return;
    }

    // Display the available travel dates for the selected route.
    for (int i = 0; i < dateCount; i++) {
        printf("%d. %s\n", i + 1, availableDates[i]);
    }

    // Ask the user to select a travel date for the trip.
    int dateChoice;
    printf("Select a travel date: ");
    // Validate the user's date choice (must be a number and within the valid date range).
    if (scanf("%d", &dateChoice) != 1 || dateChoice < 1 || dateChoice > dateCount) {
        printf("Invalid date selection.\n");
        return;  // Exit if the date selection is invalid.
    }

    // Copy the selected travel date to the travelDate variable.
    strcpy(travelDate, availableDates[dateChoice - 1]);

    // Search for the bus index that matches the selected bus route and travel date.
    int busIndex = -1;
    for (int i = 0; i < busCount; i++) {
        if (strcmp(buses[i].busNumberPlate, selectedBusNumberPlate) == 0 &&
            strcmp(buses[i].source, selectedSource) == 0 &&
            strcmp(buses[i].destination, selectedDestination) == 0 &&
            strcmp(buses[i].date, travelDate) == 0) {
            busIndex = i;  // Found the matching bus, store its index.
            break;
        }
    }

    // If no matching bus is found for the selected travel date, display an error message and exit.
    if (busIndex == -1) {
        printf("Error: No matching bus found for the selected travel date.\n");
        return;
    }

    // Now we will display the available seats for the selected bus on the chosen travel date.
    printf("\nAvailable seats for One-Way Trip:\n");
    showSeats(buses[busIndex]);  // Call the showSeats function to display the available seats.

    int seatNumbers[MAX_SEATS], numSeats;
    // Arrays to store booking details for up to 2 trips (for round-trip bookings)
    int ticketNumbers[2];  // Stores ticket numbers for each trip
    float totalFares[2];   // Stores fare details for each trip
    int busIndices[2];     // Stores the indices of selected buses for each trip


    // Call the bookSeat function to allow the user to select seats for the one-way trip.
    // If the user successfully books seats, finalize the booking.
    if (bookSeat(currentUser, &buses[busIndex], seatNumbers, &numSeats) > 0) {
        finalizeBooking(currentUser, buses, busCount, busIndex, numSeats, seatNumbers, bookingDate, 0, 1, ticketNumbers, totalFares, busIndices);  // Finalize the outbound trip booking
    } else {
        printf("No seats booked for outbound trip.\n");
        return;
    }

    // Check if the user is booking a round-trip (two frequent trips).
    int returnBusIndex = -1;  // Initialize return bus index to -1 (not yet found).

    if (tripCount == 2) {  // If the user has two frequent trips (round-trip), prompt for return booking.
        char confirmRoundTrip;
        printf("\nWould you like to book a return trip as well? (Y/N): ");
        scanf(" %c", &confirmRoundTrip);  // Ask the user if they want to book a return trip.

        // If the user wants a return trip, find available dates and allow them to select a return date.
        if (confirmRoundTrip == 'Y' || confirmRoundTrip == 'y') {
            printf("\nAvailable Return Dates:\n");
            char returnAvailableDates[MAX_BOOKINGS][11];
            int returnDateCount = 0;

            // Iterate through all buses to find return trips (destination and source are reversed).
            for (int i = 0; i < busCount; i++) {
                if (strcmp(buses[i].busNumberPlate, frequentBusNumberPlates[1]) == 0 &&
                    strcmp(buses[i].source, destinations[0]) == 0 &&
                    strcmp(buses[i].destination, sources[0]) == 0) {
                    strcpy(returnAvailableDates[returnDateCount], buses[i].date);  // Store return dates.
                    returnDateCount++;  // Increment return date counter.
                }
            }

            // If no return trips are found, display an error message and exit.
            if (returnDateCount > 0) {
                // Display the available return dates.
                for (int i = 0; i < returnDateCount; i++) {
                    printf("%d. %s\n", i + 1, returnAvailableDates[i]);
                }

                // Prompt the user to select a return date.
                int returnDateChoice;
                printf("Select a return date: ");
                // Validate the user's return date selection.
                if (scanf("%d", &returnDateChoice) != 1 || returnDateChoice < 1 || returnDateChoice > returnDateCount) {
                    printf("Invalid return date selection.\n");
                    return;  // Exit if the selection is invalid.
                }

                // Copy the selected return date to the returnTravelDate variable.
                strcpy(returnTravelDate, returnAvailableDates[returnDateChoice - 1]);

                // Find the bus index for the return trip based on selected details.
                for (int i = 0; i < busCount; i++) {
                    if (strcmp(buses[i].busNumberPlate, frequentBusNumberPlates[1]) == 0 &&
                        strcmp(buses[i].source, destinations[0]) == 0 &&
                        strcmp(buses[i].destination, sources[0]) == 0 &&
                        strcmp(buses[i].date, returnTravelDate) == 0) {
                        returnBusIndex = i;  // Found the return bus, store its index.
                        break;
                    }
                }

                // If the return bus is found, display available seats and allow the user to book them.
                if (returnBusIndex != -1) {
                    printf("\nAvailable seats for Return Trip:\n");
                    showSeats(buses[returnBusIndex]);  // Show return trip available seats.

                    // Declare an array to store return trip seat selections.
                    int returnSeatNumbers[MAX_SEATS], returnNumSeats;
                    // Call the bookSeat function for the return trip.
                    if (bookSeat(currentUser, &buses[returnBusIndex], returnSeatNumbers, &returnNumSeats) > 0) {
                        finalizeBooking(currentUser, buses, busCount, busIndex, numSeats, seatNumbers,bookingDate, 1, 2, ticketNumbers, totalFares, busIndices);
                    } else {
                        printf("No seats booked for return trip.\n");
                    }
                } else {
                    printf("No available return buses for this route.\n");
                }
            } else {
                printf("No return trips available.\n");
            }
        }
    }
}

void finalizeBooking(struct user currentUser, struct BusReservation buses[], int busCount, int busIndex, int numSeats, int seatNumbers[], char *bookingDate, int tripIndex, int totalTrips, int *ticketNumbers, float *totalFares, int *busIndices) {
    int ticketNumber = generateTicketNumber(); // Generate a new unique ticket number for this booking
    float baseFare = numSeats * buses[busIndex].fare; // Calculate the base fare for the selected number of seats
    float sst = baseFare * SST_RATE;  // Compute the 6% Sales & Service Tax (SST) based on the base fare
    float finalAmount = baseFare + sst; // Calculate the total amount the user needs to pay (base fare + SST)

    // Store the generated ticket number, final amount, and bus index for this trip
    ticketNumbers[tripIndex] = ticketNumber;
    totalFares[tripIndex] = finalAmount;
    busIndices[tripIndex] = busIndex;  // Store bus index for reference

    // Declare an array to store seat counts for each trip
    static int seatCounts[2];
    seatCounts[tripIndex] = numSeats;  // Store the number of seats for this trip

    // Declare and allocate memory for seat numbers for each trip
    static int *seatNumbersForAllTrips[2];
    seatNumbersForAllTrips[tripIndex] = (int *)malloc(numSeats * sizeof(int));

    if (seatNumbersForAllTrips[tripIndex] == NULL) {
        printf("Memory allocation failed for seat numbers.\n");
        return;
    }

    // Copy the seat numbers to the allocated array
    for (int i = 0; i < numSeats; i++) {
        seatNumbersForAllTrips[tripIndex][i] = seatNumbers[i];
    }

    // If this is the last trip (i.e., the final booking in case of round trips)
    if (tripIndex == totalTrips - 1) {
        printf("\n======================================\n");
        printf("           BOOKING SUMMARY          \n");
        printf("======================================\n");

        float totalPayment = 0; // Variable to accumulate total fare for all trips

        // Loop through all booked trips and print details
        for (int i = 0; i < totalTrips; i++) {
            int currentBusIndex = busIndices[i];  // Get the correct bus index for this trip
            float tripBaseFare = numSeats * buses[currentBusIndex].fare;
            float tripSST = tripBaseFare * SST_RATE;

            // Display ticket and trip details
            printf("Ticket Number: %d\n", ticketNumbers[i]);
            printf("Bus ID: %d\n", buses[currentBusIndex].busID);
            printf("Bus Plate: %s\n", buses[currentBusIndex].busNumberPlate);
            printf("Route: %s → %s\n", buses[currentBusIndex].source, buses[currentBusIndex].destination);
            printf("Booking Date: %s\n", bookingDate);
            printf("--------------------------------------\n");
            printf("Seats: ");
            for (int j = 0; j < seatCounts[i]; j++) {
                printf("%d", seatNumbersForAllTrips[i][j]);
                if (j < seatCounts[i] - 1) printf(" ");
            }
            printf("\n");
            printf("\n--------------------------------------\n");
            printf("Base Fare:     RM %.2f\n", tripBaseFare);
            printf("SST (6%%):      RM %.2f\n", tripSST);
            printf("Total Amount:  RM %.2f\n", totalFares[i]);
            printf("======================================\n");

            // Accumulate total payment for all trips
            totalPayment += totalFares[i];
        }

        // If the user booked a round trip, display the total amount for both trips
        if (totalTrips > 1) {
            printf("\n======================================\n");
            printf("         FINAL PAYMENT SUMMARY      \n");
            printf("======================================\n");
            printf("Total Amount for Round Trip: RM %.2f\n", totalPayment);
            printf("======================================\n");
        }

        // Ask the user if they want to proceed with the payment
        char proceed;
        while (getchar() != '\n'); // flush input before asking
        printf("\nDo you want to proceed with the payment? (Y/N): ");
        scanf("%c", &proceed);

        if (proceed == 'y' || proceed == 'Y') {
            // Process payment once at the end of the booking process
            processPayment(totalPayment);

            // Send booking confirmation notifications via email and SMS
            struct notification notif;

            // Save reservation data after payment
            for (int i = 0; i < totalTrips; i++) {
                saveReservation(currentUser, ticketNumbers[i], buses[busIndices[i]].busID, buses[busIndices[i]].busNumberPlate, numSeats, seatNumbersForAllTrips[i], bookingDate, finalAmount);

                // Prepare and send Email Confirmation
                notif.isEmail = 1;
                strcpy(notif.recipient.email, currentUser.email);
                strcpy(notif.type, "email");
                strcpy(notif.category, "Confirmation");
                saveNotification(&notif, ticketNumbers[i]);

                // Prepare and send SMS Confirmation
                notif.isEmail = 0;
                strcpy(notif.recipient.phone, currentUser.phone);
                strcpy(notif.type, "sms");
                strcpy(notif.category, "Confirmation");
                saveNotification(&notif, ticketNumbers[i]);
            }

            // Save booking data for future reference
            saveBuses(buses, busCount);   // Save updated bus data
            saveSeats(buses, busCount);   // Save seat allocation data
            saveFrequentBooking(currentUser, buses[busIndex].busID, buses[busIndex].busNumberPlate, bookingDate, buses[busIndex].source, buses[busIndex].destination);

            // Free allocated memory after processing all trips
            for (int i = 0; i < totalTrips; i++) {
                free(seatNumbersForAllTrips[i]);
            }

            // Display final success message to the user
            printf("\nBooking successful! Enjoy your trip.\n");
        } else {
            printf("\nBooking canceled.\n");

            // Rollback the seat reservations
            for (int i = 0; i < totalTrips; i++) {
                int currentBusIndex = busIndices[i];

                // For each seat that was temporarily reserved, remove it
                for (int j = 0; j < seatCounts[i]; j++) {
                    int seatToRemove = seatNumbersForAllTrips[i][j];

                    // Find and remove the seat from reservedSeats[]
                    int foundIndex = -1;
                    for (int k = 0; k < buses[currentBusIndex].reservedCount; k++) {
                        if (buses[currentBusIndex].reservedSeats[k] == seatToRemove) {
                            foundIndex = k;
                            break;
                        }
                    }

                    if (foundIndex != -1) {
                        // Shift remaining elements to remove seat
                        for (int k = foundIndex; k < buses[currentBusIndex].reservedCount - 1; k++) {
                            buses[currentBusIndex].reservedSeats[k] = buses[currentBusIndex].reservedSeats[k + 1];
                        }
                        buses[currentBusIndex].reservedCount--;
                    }
                }

                // Restore availableSeats
                buses[currentBusIndex].availableSeats += seatCounts[i];

                // Rollback booking count if it was incremented earlier
                if (buses[currentBusIndex].bookingCount > 0) {
                    buses[currentBusIndex].bookingCount--;
                }
            }

            // Free allocated memory for seat numbers
            for (int i = 0; i < totalTrips; i++) {
                free(seatNumbersForAllTrips[i]);
            }
        }
    }
}

// Function to generate a unique 6-digit ticket number
int generateTicketNumber() {
    int ticketNumber;
    do {
        ticketNumber = 100000 + rand() % 900000; // Generate a random 6-digit ticket number (100000 to 999999)
    } while (!IsUnique(ticketNumber)); // Repeat until a unique ticket number is found

    return ticketNumber; // Return the unique ticket number
}

// Function to check if a ticket number is unique by scanning "reservation.txt"
bool IsUnique(int ticketNumber) {
    FILE *file = fopen("reservation.txt", "r"); // Open the reservation file in read mode
    if (!file) return true; // If the file doesn't exist (i.e., no prior reservations), assume the number is unique

    int existingTicket;
    char line[256]; // Buffer to store each line from the file

    // Read each line from the reservation file
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%*[^,],%d", &existingTicket); // Extract the ticket number from the line (ignoring other data)
        if (existingTicket == ticketNumber) { // If the ticket number already exists
            fclose(file); // Close the file
            return false; // Indicate that the number is not unique
        }
    }

    fclose(file); // Close the file after reading all entries
    return true; // If no matching ticket number was found, return true (unique)
}

// Function to load ticket numbers from "reservation.txt" into the bus reservation system
int loadTicketNumbers(struct BusReservation buses[], int busCount) {
    FILE *file = fopen("reservation.txt", "r"); // Open the reservation file in read mode
    if (!file) {
        printf("Warning: No previous reservations found.\n");
        return 0; // If the file doesn't exist, return 0 (no bookings loaded)
    }

    char line[256]; // Buffer to store each line from the file
    int count = 0; // Counter for the number of reservations loaded

    // Read each line from the file to extract reservation details
    while (fgets(line, sizeof(line), file)) {
        char username[50], seatNumbers[50];
        int ticketNumber, busID, numSeats;
        float totalFare;

        // Parse the reservation details from the formatted file
        if (sscanf(line, "%49[^,],%d,%d,%d,%49[^,],%f", username, &ticketNumber, &busID, &numSeats, seatNumbers, &totalFare) == 6) {
            // Find the corresponding bus in the system
            for (int i = 0; i < busCount; i++) {
                if (buses[i].busID == busID) { // If the bus ID matches, load ticket data into the system
                    int bookingIndex = buses[i].bookingCount; // Get current booking count for this bus

                    if (bookingIndex < MAX_BOOKINGS) { // Ensure we do not exceed the booking limit
                        buses[i].ticketNumbers[bookingIndex] = ticketNumber; // Store the ticket number
                        buses[i].seatCounts[bookingIndex] = numSeats; // Store the number of seats booked

                        // Parse and store seat numbers (seatNumbers is a space-separated string)
                        char *token = strtok(seatNumbers, " "); // Tokenize seat numbers
                        int seatIndex = 0;
                        while (token != NULL && seatIndex < numSeats) {
                            buses[i].bookings[bookingIndex][seatIndex++] = atoi(token); // Convert and store seat number
                            token = strtok(NULL, " "); // Move to the next seat number
                        }

                        buses[i].bookingCount++; // Increment the number of bookings for this bus
                        count++; // Increment the total reservations loaded
                    }
                    break; // Exit the loop once the bus is found
                }
            }
        }
    }

    fclose(file); // Close the reservation file
    return count; // Return the total number of reservations loaded
}

// Function to save a new reservation to "reservation.txt"
void saveReservation(struct user currentUser, int ticketNumber, int busID, char *busNumberPlate,
                     int numSeats, int seatNumbers[], char *bookingDate, float finalAmount) {
    FILE *file = fopen("reservation.txt", "a"); // Open the file in append mode to add a new entry

    if (!file) { // If the file cannot be opened
        printf("Error: Could not open file for writing!\n");
        return;
    }

    // Write reservation details in a structured format
    fprintf(file, "%s,%d,%d,%s,%s,%d,", currentUser.username, ticketNumber, busID, busNumberPlate, bookingDate, numSeats);

    // Write all seat numbers as a space-separated string
    for (int i = 0; i < numSeats; i++) {
        fprintf(file, "%d", seatNumbers[i]); // Print each seat number
        if (i < numSeats - 1) fprintf(file, " "); // Add space between seat numbers (but not after the last one)
    }

    // Write the total fare and end the line
    fprintf(file, ",%.2f\n", finalAmount);

    fclose(file); // Close the file after saving the reservation
}

// Function to retrieve and display ticket details based on user input
void getTicketDetails(struct BusReservation buses[], int busCount) {
    int ticketNumber; // Variable to store user-provided ticket number
    printf("Enter your Ticket Number: ");
    scanf("%d", &ticketNumber);

    // Open the reservation file to search for the ticket number
    FILE *file = fopen("reservation.txt", "r");
    if (!file) { // If file opening fails, display an error and exit the function
        printf("Error: Could not open reservation file!\n");
        return;
    }

    char line[256]; // Buffer to hold each line read from the file
    int found = 0;  // Flag to check if the ticket is found

    // Read each reservation entry from the file
    while (fgets(line, sizeof(line), file)) {
        char username[50], busNumberPlate[20], bookingDate[20], seatNumbers[100];
        int fileTicketNumber, busID, numSeats;
        float totalFare;

        // Parse reservation details from the file using `sscanf`
        int readCount = sscanf(line, "%49[^,],%d,%d,%19[^,],%19[^,],%d,%99[^,],%f",
                               username, &fileTicketNumber, &busID, busNumberPlate,
                               bookingDate, &numSeats, seatNumbers, &totalFare);

        // Check if the correct number of values were read and if the ticket number matches
        if (readCount == 8 && fileTicketNumber == ticketNumber) {
            found = 1; // Mark ticket as found

            struct BusReservation *bus = NULL;

            // Search for the corresponding bus in the system memory
            for (int i = 0; i < busCount; i++) {
                if (buses[i].busID == busID) {
                    bus = &buses[i];
                    break;
                }
            }

            if (bus) {
                int alreadyStored = 0; // Flag to check if ticket details are already stored

                // Check if ticket is already stored in memory to avoid duplication
                for (int i = 0; i < bus->bookingCount; i++) {
                    if (bus->ticketNumbers[i] == ticketNumber) {
                        alreadyStored = 1;
                        break;
                    }
                }

                if (!alreadyStored) {
                    // Store ticket details in the respective bus struct
                    bus->ticketNumbers[bus->bookingCount] = ticketNumber;
                    bus->seatCounts[bus->bookingCount] = numSeats;

                    // Parse and store seat numbers from string format
                    char *token = strtok(seatNumbers, " ");
                    int seatIndex = 0;
                    while (token && seatIndex < numSeats) {
                        bus->bookings[bus->bookingCount][seatIndex++] = atoi(token);
                        token = strtok(NULL, " ");
                    }

                    // Increase booking count after storing ticket details
                    bus->bookingCount++;
                }

                // Call function to display ticket details
                displayTicketDetails(bus, ticketNumber, bookingDate);
            } else {
                printf("Error: Bus ID %d not found in system memory!\n", busID);
            }
            break; // Exit the loop once the ticket is found and processed
        }
    }

    fclose(file); // Close the file after reading

    if (!found) {
        printf("No booking found for Ticket %d.\n", ticketNumber);
    }
}

// Function to display formatted ticket details
void displayTicketDetails(struct BusReservation *bus, int ticketNumber, char *bookingDate) {
    // Loop through bus bookings to find matching ticket
    for (int i = 0; i < bus->bookingCount; i++) {
        if (bus->ticketNumbers[i] == ticketNumber) {
            // Calculate the final amount based on number of seats booked and fare
            float finalAmount = calculateFare(bus->seatCounts[i], bus->fare);

            // Print ticket receipt in a structured format
            printf("\n=========================================\n");
            printf("             BUS TICKET RECEIPT          \n");
            printf("=========================================\n");
            printf(" Ticket Number   : %-12d\n", ticketNumber);
            printf(" Bus ID          : %-12d\n", bus->busID);
            printf(" Bus Number Plate: %-12s\n", bus->busNumberPlate);
            printf(" Booking Date    : %-12s\n", bookingDate);
            printf(" Departure Date  : %-12s\n", bus->date);
            printf(" Source          : %-12s\n", bus->source);
            printf(" Destination     : %-12s\n", bus->destination);
            printf(" Departure Time  : %-12s\n", bus->departureTime);
            printf(" Arrival Time    : %-12s\n", bus->arrivalTime);
            printf(" Total Seats     : %-12d\n", bus->totalSeats);
            printf(" Available Seats : %-12d\n", bus->availableSeats);
            printf("-----------------------------------------\n");

            // Print booked seats in a formatted manner (5 per row)
            printf(" Booked Seats    : ");
            for (int j = 0; j < bus->seatCounts[i]; j++) {
                printf("%2d ", bus->bookings[i][j]);
                if ((j + 1) % 5 == 0 && j != bus->seatCounts[i] - 1) {
                    printf("\n                   "); // Indent continuation rows
                }
            }
            printf("\n-----------------------------------------\n");

            // Print final fare details
            printf(" Total Fare      : RM %8.2f\n", finalAmount);
            printf("=========================================\n");
            printf("     Thank you for choosing us!         \n");
            printf("=========================================\n\n");
            return; // Exit function after displaying the ticket
        }
    }

    // If ticket is not found within the bus memory
    printf("Error: Ticket %d not found in system memory!\n", ticketNumber);
}

// Function to process refund and display refund details to the user
void processRefund(float refundAmount) {
    printf("\nRefund Details:\n");
    printf("Amount Refunded: RM %.2f\n", refundAmount);
    printf("Refund will be processed within 3-5 business days.\n");
}

// Function to log a cancellation into "cancellations.txt"
void logCancellation(char *username, int ticketNumber, int busID, char *busNumberPlate, char *date, int numSeats, int canceledSeats[], float refundAmount) {
    FILE *file = fopen("cancellations.txt", "a"); // Open file in append mode
    if (!file) {
        printf("Error: Could not open cancellations.txt for writing!\n");
        return;
    }

    // Write cancellation details to file
    fprintf(file, "%s,%d,%d,%s,%s,%d,", username, ticketNumber, busID, busNumberPlate, date, numSeats);

    // Append all canceled seat numbers
    for (int i = 0; i < numSeats; i++) {
        fprintf(file, "%d", canceledSeats[i]);
        if (i < numSeats - 1) fprintf(file, " "); // Separate seat numbers with a space
    }

    fprintf(file, ",%.2f\n", refundAmount); // Append refund amount
    fclose(file); // Close file after writing
}

// Function to update files after a cancellation
void updateFilesAfterCancellation(struct BusReservation buses[], int busCount, int busID, int numSeats, int ticketNumber, char *username) {
    // Step 1: Remove canceled reservation from "reservation.txt"
    FILE *file = fopen("reservation.txt", "r"); // Open the reservation file in read mode to process existing bookings
    FILE *tempFile = fopen("temp.txt", "w"); // Open a temporary file in write mode to store updated reservations

    // Check if files opened successfully
    if (!file || !tempFile) {
        printf("Error: Unable to access reservation file.\n");
        return;
    }

    char line[256];  // Buffer to store each line read from the file
    int found = 0;   // Flag to track if a reservation has been removed

    // Read each line from the original reservation file
    while (fgets(line, sizeof(line), file)) {
        char fileUsername[50];  // Stores username extracted from the line
        int fileTicketNumber;   // Stores ticket number extracted from the line

        // Extract username and ticket number from the current line
        // The expected format in reservation.txt: username,ticketNumber,...
        if (sscanf(line, "%49[^,],%d", fileUsername, &fileTicketNumber) == 2) {
            // If this line matches the ticket to be canceled, remove it
            if (fileTicketNumber == ticketNumber && strcmp(fileUsername, username) == 0) {
                found = 1;  // Set flag to indicate a reservation was found and removed

                // Open cancellations.txt in append mode to store the removed reservation
                FILE *cancelFile = fopen("cancellations.txt", "a");
                if (cancelFile) {
                    fputs(line, cancelFile);  // Save canceled reservation to cancellations file
                    fclose(cancelFile);       // Close cancellations file after writing
                }
                continue;  // Skip writing this reservation to the new file
            }
        }
        // If this is not the canceled reservation, write it back to the temporary file
        fputs(line, tempFile);
    }

    // Close both files after processing
    fclose(file);
    fclose(tempFile);

    // Replace the original reservation file with the updated temporary file
    remove("reservation.txt");      // Delete old reservation file
    rename("temp.txt", "reservation.txt");  // Rename temp file to reservation.txt

    // Notify the user whether the cancellation was successful or not
    if (found) {
        printf("Reservation canceled successfully.\n");
    } else {
        printf("Error: Reservation not found.\n");
    }

    // Step 2: Update "seats.txt" to reflect newly available seats
    FILE *seatFile = fopen("seats.txt", "w");
    for (int i = 0; i < busCount; i++) {
        fprintf(seatFile, "%d,%d", buses[i].busID, buses[i].reservedCount);
        for (int j = 0; j < buses[i].reservedCount; j++) {
            fprintf(seatFile, ",%d", buses[i].reservedSeats[j]); // Write each reserved seat
        }
        fprintf(seatFile, "\n");
    }
    fclose(seatFile);

    // Step 3: Update "buses.txt" to reflect available seats after cancellation
    FILE *busFile = fopen("buses.txt", "r");
    char busLines[MAX_BUSES][256]; // Buffer for storing file lines
    int busIndex = 0;

    // Read each line of "buses.txt"
    while (fgets(busLines[busIndex], sizeof(busLines[busIndex]), busFile)) {
        int fileBusID, totalSeats, availableSeats;
        char busPlate[50], date[50], source[50], dest[50], depart[10], arrive[10];
        float fare;

        // Extract bus details from line
        if (sscanf(busLines[busIndex], "%d,%49[^,],%49[^,],%49[^,],%49[^,],%9[^,],%9[^,],%d,%d,%f",
                   &fileBusID, busPlate, date, source, dest, depart, arrive, &totalSeats, &availableSeats, &fare) == 10) {
            // If the current bus ID matches the canceled ticket, update available seats
            if (fileBusID == busID) {
                sprintf(busLines[busIndex], "%d,%s,%s,%s,%s,%s,%s,%d,%d,%.2f\n",
                        fileBusID, busPlate, date, source, dest, depart, arrive, totalSeats, availableSeats + numSeats, fare);
            }
        }

        if (busIndex < MAX_BUSES) { // Prevent overflow
            busIndex++;
        }
    }
    fclose(busFile); // Close file after reading

    // Write back updated bus information to "buses.txt"
    busFile = fopen("buses.txt", "w");
    for (int i = 0; i < busIndex; i++) {
        fputs(busLines[i], busFile);
    }
    fclose(busFile);
}

// Function to cancel a booking based on the ticket number
void cancelBooking(struct user currentUser, struct BusReservation buses[], int busCount) {
    int ticketNumber;

    // Prompt user to enter the ticket number they want to cancel
    printf("Enter your Ticket Number to cancel: ");
    scanf("%d", &ticketNumber);

    // Open the reservation file to search for the booking
    FILE *file = fopen("reservation.txt", "r");
    if (!file) {
        printf("Error: No reservations found!\n");
        return;
    }

    // Variables to store booking details
    int found = 0, busID = -1, numSeats = 0;
    float refundAmount = 0.0;
    int canceledSeats[MAX_SEATS];
    char username[50], busPlate[20], date[20];

    // Iterate through the file to locate the ticket
    while (!feof(file)) {
        int fileTicketNumber, fileBusID, fileNumSeats;
        float totalFare;

        // Read user booking details from file
        if (fscanf(file, "%49[^,],%d,%d,%19[^,],%19[^,],%d,", username, &fileTicketNumber, &fileBusID, busPlate, date, &fileNumSeats) != 6) {
            fscanf(file, "%*[^\n]\n");  // Skip the invalid line to prevent an infinite loop
            continue;
        }

        // Read seat numbers from file
        for (int i = 0; i < fileNumSeats; i++) {
            fscanf(file, "%d", &canceledSeats[i]);
        }

        // Read the total fare at the end of the line
        fscanf(file, ",%f\n", &totalFare);

        // If the ticket number and username match, store the relevant details
        if (fileTicketNumber == ticketNumber && strcmp(username, currentUser.username) == 0) {
            found = 1;         // Mark booking as found
            busID = fileBusID; // Store bus ID
            numSeats = fileNumSeats; // Store number of seats booked
            refundAmount = totalFare; // Store refund amount
            break; // Stop searching once the correct ticket is found
        }
    }
    fclose(file); // Close the file after reading

    // If no booking is found, notify the user and exit function
    if (!found) {
        printf("Error: No booking found with Ticket %d for user %s.\n", ticketNumber, currentUser.username);
        return;
    }

    // Ask for confirmation before proceeding with the cancellation
    char confirm;
    printf("\nAre you sure you want to cancel Ticket %d? (Y/N): ", ticketNumber);
    scanf(" %c", &confirm); // Space before %c ensures it reads input correctly after previous scanf

    // If user does not confirm, cancel the process
    if (confirm != 'Y' && confirm != 'y') {
        printf("Cancellation aborted.\n");
        return;
    }

    // Create a notification struct for email notification
    struct notification notif;

    notif.isEmail = 1;  // Set type to email notification
    strcpy(notif.recipient.email, currentUser.email);
    strcpy(notif.type, "email");
    strcpy(notif.category, "Cancellation");

    // Save the email notification
    saveNotification(&notif, ticketNumber);

    // Create another notification struct for SMS notification
    notif.isEmail = 0;  // Set type to SMS notification
    strcpy(notif.recipient.phone, currentUser.phone);
    strcpy(notif.type, "sms");
    strcpy(notif.category, "Cancellation");

    // Save the SMS notification
    saveNotification(&notif, ticketNumber);

    // Iterate through the buses to locate the bus with the given ID
    for (int i = 0; i < busCount; i++) {
        if (buses[i].busID == busID) {
            // Remove the canceled seats from the bus's reserved seat list
            for (int j = 0; j < numSeats; j++) {
                for (int k = 0; k < buses[i].reservedCount; k++) {
                    if (buses[i].reservedSeats[k] == canceledSeats[j]) {
                        // Shift the remaining seats to the left after removal
                        for (int l = k; l < buses[i].reservedCount - 1; l++) {
                            buses[i].reservedSeats[l] = buses[i].reservedSeats[l + 1];
                        }
                        buses[i].reservedCount--; // Reduce the reserved seat count
                        break; // Move to the next seat to remove
                    }
                }
            }
            // Update the available seat count after cancellation
            buses[i].availableSeats += numSeats;
            break; // Stop once the correct bus is updated
        }
    }

    // Record the cancellation details in cancellations.txt
    logCancellation(currentUser.username, ticketNumber, busID, busPlate, date, numSeats, canceledSeats, refundAmount);

    // Remove the booking entry from reservation.txt and update all related files
    updateFilesAfterCancellation(buses, busCount, busID, numSeats, ticketNumber, currentUser.username);

    // Notify the user of successful cancellation and process refund
    printf("Booking canceled successfully! Processing refund...\n");
    processRefund(refundAmount);
}

struct user getUserDetails(const char *username) {
    // Iterate through the users array to find a matching username
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return users[i];  // Return the found user structure
        }
    }

    // If no matching user is found, return an empty user structure with blank fields
    struct user emptyUser = {"", "", "", "", ""};
    return emptyUser;
}

void saveNotification(struct notification *notif, int ticketNumber) {
    FILE *file = NULL;  // File pointer for writing notification data

    // Determine the correct file based on the notification type
    if (notif->isEmail) {
        file = fopen("email.txt", "a");  // Open email.txt in append mode
        if (!file) {
            printf("Error: Could not open email.txt for writing.\n");
            return;  // Exit the function if file opening fails
        }
    }
    else {  // If it's an SMS notification
        file = fopen("sms.txt", "a");  // Open sms.txt in append mode
        if (!file) {
            printf("Error: Could not open sms.txt for writing.\n");
            return;  // Exit the function if file opening fails
        }
    }

    // Write notification details to the corresponding file
    // Format: recipient (email or phone), type (email/sms), category (Cancellation/Booking), ticket number
    fprintf(file, "%s, %s - %s - %d\n",
            notif->isEmail ? notif->recipient.email : notif->recipient.phone,
            notif->type, notif->category, ticketNumber);

    // Close the file after writing to ensure data is saved properly
    fclose(file);
}

void checkAndRemoveUserUpdate(const char *currentUser) {
    // Open temp_updates.txt in read mode to check if the user has pending updates
    FILE *file = fopen("temp_updates.txt", "r");
    if (!file) {
        printf("No pending updates for user.\n");
        return;  // If the file doesn't exist, return early
    }

    char username[USERNAME_LENGTH];  // Buffer to store usernames from the file
    int found = 0;  // Flag to track if the user was found

    // Open a temporary file to store updated user list without the current user
    FILE *temp = fopen("tempfile.txt", "w");
    if (!temp) {
        perror("Error opening tempfile.txt");
        fclose(file);
        return;
    }

    // Read each username from the file and process accordingly
    while (fgets(username, sizeof(username), file)) {
        username[strcspn(username, "\n")] = 0;  // Remove the newline character

        // If the username matches the current user, set flag but don't write it back
        if (strcmp(username, currentUser) == 0) {
            found = 1;
        } else {
            fprintf(temp, "%s\n", username);  // Write other users back to temp file
        }
    }

    fclose(file);
    fclose(temp);

    // If the user had an update, notify them and update the file accordingly
    if (found) {
        printf("==============================================================================\n");
        printf(" IMPORTANT NOTICE: Your Bus Schedule Has Been Updated \n");
        printf("==============================================================================\n");
        printf("Dear Customer,\n");
        printf("There has been a change to your bus schedule. \n");
        printf("Please check your updated booking details in your email or SMS notifications.\n");
        printf("For any queries, feel free to contact our support team.\n");
        printf("Thank you for choosing our service!\n");
        printf("==============================================================================\n");

        // Remove the old file since the user was notified
        if (remove("temp_updates.txt") != 0) {
            perror("Error removing temp_updates.txt");
        }

        // Rename the temporary file to replace the old update list
        if (rename("tempfile.txt", "temp_updates.txt") != 0) {
            perror("Error renaming tempfile.txt");
        }
    } else {
        // If no update was found for the user, delete the temporary file
        remove("tempfile.txt");
    }
}

void appendUserToTempFile(const char *username) {
    // Open temp_updates.txt in append mode to add the user to the update list
    FILE *tempFile = fopen("temp_updates.txt", "a");
    if (!tempFile) {
        perror("Error opening temp_updates.txt");
        return;
    }

    // Append the username to the file
    fprintf(tempFile, "%s\n", username);
    fclose(tempFile);
}

void notifyUser(const char *username, int ticketNumber) {
    // Fetch user details from the system
    struct user bookedUser = getUserDetails(username);
    struct notification notif;  // Notification structure

    // If the user has an email, send an email notification
    if (strlen(bookedUser.email) > 0) {
        notif.isEmail = 1;
        strcpy(notif.recipient.email, bookedUser.email);
        strcpy(notif.type, "email");
        strcpy(notif.category, "Update");
        saveNotification(&notif, ticketNumber);
    }

    // If the user has a phone number, send an SMS notification
    if (strlen(bookedUser.phone) > 0) {
        notif.isEmail = 0;
        strcpy(notif.recipient.phone, bookedUser.phone);
        strcpy(notif.type, "sms");
        strcpy(notif.category, "Update");
        saveNotification(&notif, ticketNumber);
    }
}

void notifyUsersOfBusUpdate(int busID, const char *oldValue, const char *newValue) {
    // Open the reservation file to find users who booked the affected bus
    FILE *file = fopen("reservation.txt", "r");
    if (!file) {
        printf("Error: Could not open reservation file!\n");
        return;
    }

    char username[USERNAME_LENGTH], busNumberPlate[20], bookingDate[20], seatNumbers[100];
    int ticketNumber, bookedBusID, numSeats;
    float finalAmount;

    // Scan the file for matching bus reservations and notify users
    while (fscanf(file, "%49[^,],%d,%d,%19[^,],%19[^,],%d,%99[^,],%f\n",
                  username, &ticketNumber, &bookedBusID, busNumberPlate, bookingDate, &numSeats, seatNumbers, &finalAmount) == 8) {
        if (bookedBusID == busID) {
            appendUserToTempFile(username);  // Store the user for later updates
            logBusUpdate(busID, ticketNumber, oldValue, newValue);  // Log the change
            notifyUser(username, ticketNumber);  // Notify the user about the update
        }
    }

    fclose(file);
}

void viewNotifications(struct user *currentUser) {
    int choice; // Variable to store user's choice

    // Display the notification menu
    printf("\n====================================================\n");
    printf("                  VIEW NOTIFICATIONS               \n");
    printf("====================================================\n");
    printf("1. Email Inbox\n");
    printf("2. SMS Inbox\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice); // Read user input

    // Handle user choice and call the respective function
    if (choice == 1) {
        displayNotification("email.txt", "Email", currentUser); // Display email notifications of current user
    } else if (choice == 2) {
        displayNotification("sms.txt", "SMS", currentUser); // Display SMS notifications of current user
    } else {
        printf("Exiting notification view.\n"); // Exit if the user selects option 3
    }
}

void displayNotification(const char *filename, const char *type, struct user *currentUser) {
    // Open the specified notification file for reading
    FILE *file = fopen(filename, "r");
    if (!file) { // If file doesn't exist or can't be opened
        printf("\nNo %s notifications found!\n", type);
        return;
    }

    struct notification notifications[MAX_NOTIFICATIONS]; // Array to store notifications
    int ticketNumbers[MAX_NOTIFICATIONS]; // Array to store corresponding ticket numbers
    char buffer[300]; // Buffer for reading each line from the file
    int count = 0; // Counter for notifications

    // Display the inbox header
    printf("\n====================================================\n");
    printf("                  %s INBOX                         \n", type);
    printf("====================================================\n");
    printf(" No.  |       Subject - Ticket Number              \n");
    printf("====================================================\n");

    // Read notifications from the file line by line
    while (fgets(buffer, sizeof(buffer), file) && count < MAX_NOTIFICATIONS) {
        struct notification *notif = &notifications[count];

        // Parse the line into respective fields
        int matched = sscanf(buffer, "%49[^,], %9s - %19s - %d",
                             notif->recipient.email, notif->type, notif->category, &ticketNumbers[count]);

        if (matched < 4) continue; // Skip invalid or incorrectly formatted lines

        notif->isEmail = (strcmp(notif->type, "email") == 0); // Determine if it's an email

        // 🔍 Only display if the email or phone matches the current user
        if ((notif->isEmail && strcmp(notif->recipient.email, currentUser->email) == 0) ||
            (!notif->isEmail && strcmp(notif->recipient.phone, currentUser->phone) == 0)) {

            // Display notification summary in the inbox
            printf(" %-3d  | %-10s - %d\n", count + 1, notif->category, ticketNumbers[count]);
            count++;
        }
    }

    fclose(file); // Close the file after reading all notifications

    printf("====================================================\n");

    // If no notifications were found, inform the user and return
    if (count == 0) {
        printf("\nNo notifications available.\n");
        return;
    }

    int selected;
    printf("\nEnter the number of the %s you want to view (or 0 to exit): ", type);

    // Read user selection and validate input
    if (scanf("%d", &selected) != 1 || selected < 1 || selected > count) {
        printf("Returning to main menu.\n");
        return;
    }

    struct notification *selectedNotif = &notifications[selected - 1];
    int selectedTicket = ticketNumbers[selected - 1]; // Retrieve the correct ticket number

    printf("\n====================================================\n");

    // Display the full notification based on type
    if (selectedNotif->isEmail) {
        printEmailMessage(selectedNotif->category, selectedNotif->recipient.email, selectedTicket);
    } else {
        printSmsMessage(selectedNotif->category, selectedNotif->recipient.phone, selectedTicket);
    }

    printf("====================================================\n\n");
}

void printEmailMessage(const char *category, const char *recipient, int ticketNumber) {
    // Display email message format
    printf("                   EMAIL MESSAGE                   \n");
    printf("====================================================\n");
    printf("From   : noreply@busbooking.com\n");
    printf("To     : %s\n", recipient);
    printf("Subject: %s Notification - Ticket %d\n", category, ticketNumber);
    printf("----------------------------------------------------\n");

    // Determine the content based on notification category
    if (strncmp(category, "Confirmation", 12) == 0) {
        printf("Dear Customer,\n\n");
        printf("Your bus booking has been confirmed.\n");
        printf("Ticket Number: %d\n\n", ticketNumber);
        printf("Please view your ticket for further details.\n\n");
        printf("Ensure you carry a valid ID while traveling.\n\n");
    }
    else if (strncmp(category, "Cancellation", 12) == 0) {
        printf("Dear Customer,\n\n");
        printf("We regret to inform you that your bus booking has been cancelled.\n");
        printf("Ticket Number: %d\n\n", ticketNumber);
        printf("Any applicable refunds will be processed shortly.\n");
        printf("For further assistance, please contact our customer support.\n\n");
    }
    else if (strncmp(category, "Update", 6) == 0) {
        printf("Dear Customer,\n");
        printf("There has been an update to your bus schedule.\n");
        printf("Ticket Number: %d\n\n", ticketNumber);

        // Attempt to open the file "updates.txt" in read mode ("r")
        FILE *file = fopen("updates.txt", "r");

        // Check if the file was successfully opened
        if (file) {
            // Declare variables to store extracted data from each line of the file
            char field[50], oldValue[50], newValue[50];  // Strings to hold field name, old value, and new value
            int fileTicket, fileBus;  // Integers to hold the ticket number and bus ID
            int found = 0;  // Flag to indicate whether any matching updates were found

            // Print header to indicate that updates will be displayed
            printf("Recent Updates:\n");

            // Read each line of the file and extract relevant details
            // The expected format of each line in updates.txt:
            // ticketNumber, busID, oldValue, newValue
            while (fscanf(file, "%d, %d, %49[^,], %49[^\n]\n",
                        &fileTicket, &fileBus, oldValue, newValue) == 4) {
                // Check if the ticket number in the file matches the user's ticket number
                if (fileTicket == ticketNumber) {
                    // If a match is found, print the update details
                    printf("Changes made: '%s' -> '%s'\n", oldValue, newValue);

                    // Set the flag to indicate that at least one update was found
                    found = 1;
                }
            }

            // Close the file after reading all the necessary data
            fclose(file);
        }
    }

    printf("\nBest regards,\n");
    printf("Bus Reservation System Team\n");
}

void printSmsMessage(const char *category, const char *recipient, int ticketNumber) {
    // Display SMS message format
    printf("                   SMS MESSAGE                      \n");
    printf("====================================================\n");
    printf("From: Bus Reservation System\n");
    printf("To  : %s\n", recipient);
    printf("----------------------------------------------------\n");

    // Determine the content based on notification category
    if (strncmp(category, "Confirmation", 12) == 0) {
        printf("Dear Customer,\n");
        printf("Your booking (Ticket %d) has been confirmed.\n\n", ticketNumber);
        printf("Please view your ticket for further details.\n\n");
    }
    else if (strncmp(category, "Cancellation", 12) == 0) {
        printf("Dear Customer,\n");
        printf("Your booking (Ticket %d) has been cancelled.\n\n", ticketNumber);
        printf("Any applicable refunds will be processed soon.\n\n");
    }
    else if (strncmp(category, "Update", 6) == 0) {
        printf("Your bus schedule (Ticket %d) has been updated.\n", ticketNumber);

        // Attempt to open the file "updates.txt" in read mode ("r")
        FILE *file = fopen("updates.txt", "r");

        // Check if the file was successfully opened
        if (file) {
            // Declare variables to store extracted data from each line of the file
            char field[50], oldValue[50], newValue[50];  // Strings to hold field name, old value, and new value
            int fileTicket, fileBus;  // Integers to hold the ticket number and bus ID
            int found = 0;  // Flag to indicate whether any matching updates were found

            // Print header to indicate that updates will be displayed
            printf("Recent Updates:\n");

            // Read each line of the file and extract relevant details
            // The expected format of each line in updates.txt:
            // ticketNumber, busID, oldValue, newValue
            while (fscanf(file, "%d, %d, %49[^,], %49[^\n]\n",
                        &fileTicket, &fileBus, oldValue, newValue) == 4) {
                // Check if the ticket number in the file matches the user's ticket number
                if (fileTicket == ticketNumber) {
                    // If a match is found, print the update details
                    printf("Changes made: '%s' -> '%s'\n", oldValue, newValue);

                    // Set the flag to indicate that at least one update was found
                    found = 1;
                }
            }

            // Close the file after reading all the necessary data
            fclose(file);
        }
    }
}

void generateReports(struct BusReservation buses[], int busCount) {
    // Ensure that required files exist before generating reports.
    // The function checks if "reservation.txt", "cancellations.txt",
    // and "buses.txt" exist. If any of them is missing, it creates an empty file.

    FILE *tempFile;
    const char *files[] = {"reservation.txt", "cancellations.txt", "buses.txt"};

    for (int i = 0; i < 3; i++) {
        tempFile = fopen(files[i], "r");  // Try opening the file in read mode
        if (!tempFile) {
            // If the file does not exist, create it in write mode
            tempFile = fopen(files[i], "w");
            fclose(tempFile);
            tempFile = fopen(files[i], "r");  // Reopen it in read mode
        }
        fclose(tempFile);  // Close the file after ensuring its existence
    }

    // Generate reports: bus-wise and user-based
    generateBusReport(buses, busCount);
    generateUserReport();
}

void generateBusReport(struct BusReservation buses[], int busCount) {
    // Open required files for reading reservation and cancellation data.
    // Also, open "bus_report.txt" in write mode to store the generated report.

    FILE *reservationFile = fopen("reservation.txt", "r");
    FILE *cancellationFile = fopen("cancellations.txt", "r");
    FILE *reportFile = fopen("bus_report.txt", "w");

    // Check if any file failed to open and handle errors accordingly.
    if (!reservationFile || !cancellationFile || !reportFile) {
        printf("Error: Could not open one or more files.\n");
        if (reservationFile) fclose(reservationFile);
        if (cancellationFile) fclose(cancellationFile);
        if (reportFile) fclose(reportFile);
        return;
    }

    // Sort buses by busNumberPlate in alphabetical order.
    // If two buses have the same busNumberPlate, sort them by date.
    for (int i = 0; i < busCount - 1; i++) {  // Outer loop iterates through each bus
        for (int j = i + 1; j < busCount; j++) {  // Inner loop compares the current bus with the remaining buses

            // Compare bus number plates (lexicographical order)
            if (strcmp(buses[i].busNumberPlate, buses[j].busNumberPlate) > 0 ||
                // If bus number plates are the same, compare dates (earlier date comes first)
                (strcmp(buses[i].busNumberPlate, buses[j].busNumberPlate) == 0 &&
                 strcmp(buses[i].date, buses[j].date) > 0)) {

                // Swap buses[i] and buses[j] if they are out of order
                struct BusReservation temp = buses[i];
                buses[i] = buses[j];
                buses[j] = temp;
            }
        }
    }

    // Iterate over each bus in the list to compute its report data
    for (int i = 0; i < busCount; i++) {
        struct BusReservation *bus = &buses[i]; // Pointer to the current bus entry

        // Initialize statistics for the current bus
        int totalBookings = 0;       // Count of successful bookings for this bus
        int totalCancellations = 0;  // Count of canceled bookings for this bus
        int totalBookedSeats = 0;   // Counter for the total number of booked seats
        int totalCanceledSeats = 0; // Counter for the total number of canceled seats
        float totalRevenue = 0;      // Total revenue from all bookings (before refunds)
        float lostRevenue = 0;       // Amount lost due to cancellations (refunds issued)
        float netRevenue = 0;        // Actual revenue after subtracting lost revenue

        // Read the reservations file and count valid bookings for the current bus.
        rewind(reservationFile);
        char line[MAX_LINE];

        // Read each line from the "reservation.txt" file to analyze bookings
        while (fgets(line, sizeof(line), reservationFile)) {
            int fileBusID;      // Holds the bus ID from the current reservation entry
            float amountPaid;   // Stores the fare paid for the ticket
            int numSeats;       // Number of seats booked for this reservation

            // Extract relevant details from the reservation entry using `sscanf`
            sscanf(line, "%*[^,],%*d,%d,%*[^,],%*[^,],%d,%*[^,],%f", &fileBusID, &numSeats, &amountPaid);
            // Check if the extracted bus ID matches the current bus
            if (fileBusID == bus->busID) {
                totalBookings++;       // Increment booking count for this bus
                totalBookedSeats += numSeats;  // Add booked seats to total
                netRevenue += amountPaid; // Add fare to total net revenue
            }
        }

        // Read the cancellations file and count cancellations for the current bus.
        rewind(cancellationFile);

        // Read each line from the "cancellations.txt" file to analyze cancellation
        while (fgets(line, sizeof(line), cancellationFile)) {
            int fileBusID;  // Holds the bus ID from the current reservation entry
            float refund;   // Stores the fare paid for the ticket
            int numSeatsCanceled; // Number of seats canceled in this cancellation

            // Extract relevant details from the cancellation entry
            sscanf(line, "%*[^,],%*d,%d,%*[^,],%*[^,],%d,%*[^,],%f", &fileBusID, &numSeatsCanceled, &refund);

            // Check if the extracted bus ID matches the current bus
            if (fileBusID == bus->busID) {
                totalCancellations++;       // Increment cancellation count for this bus
                totalCanceledSeats += numSeatsCanceled;  // Add canceled seats to total
                lostRevenue += refund;      // Add refund to total lost revenue
            }
        }

        // Compute total revenue, including both earned and lost revenue.
        totalRevenue = netRevenue + lostRevenue;

        // Write bus-specific report data to "bus_report.txt".
        fprintf(reportFile, "%d,%s,%s,%d,%d,%d,%d,RM %.2f,RM %.2f,RM %.2f\n",
            bus->busID, bus->busNumberPlate, bus->date, totalBookings, totalCancellations,
            totalBookedSeats, totalCanceledSeats, totalRevenue, lostRevenue, netRevenue);
    }

    // Close all opened files to release resources.
    fclose(reservationFile);
    fclose(cancellationFile);
    fclose(reportFile);
}

void printReportHeader() {
    // Print a structured header for displaying the report in tabular format.
    printf("=====================================================================================================================================================\n");
    printf("| %-5s | %-12s | %-10s | %-12s | %-12s | %-13s | %-13s | %-13s | %-13s | %-13s |\n",
           "ID", "Number Plate", "Date", "Bookings", "Cancellations", "Booked Seats", "Canceled Seats", "Total Revenue", "Lost Revenue", "Net Revenue");
    printf("=====================================================================================================================================================\n");
}

void printBusReport() {
    // Open the bus report file for reading
    FILE *reportFile = fopen("bus_report.txt", "r");

    // Check if the file was opened successfully
    if (!reportFile) {
        printf("Error: Could not open bus_report.txt\n");
        return;
    }

    // Print the report header for tabular display
    printReportHeader();

    char line[MAX_LINE]; // Buffer to store each line read from the file

    // Read each line from the file and parse it into a BusReservation structure
    while (fgets(line, sizeof(line), reportFile)) {
        struct BusReservation bus;

        // Extract data from the CSV format line
        sscanf(line, "%d,%[^,],%[^,],%d,%d,%d,%d,RM %f,RM %f,RM %f",
               &bus.busID, bus.busNumberPlate, bus.date, &bus.totalBookings,
               &bus.totalCancellations, &bus.totalBookedSeats, &bus.totalCanceledSeats,
               &bus.totalRevenue, &bus.lostRevenue, &bus.netRevenue);

        // Print the extracted data in a formatted table row
        printf("| %-5d | %-12s | %-10s | %-12d | %-13d | %-13d | %-14d | RM %-10.2f | RM %-10.2f | RM %-10.2f |\n",
               bus.busID, bus.busNumberPlate, bus.date, bus.totalBookings,
               bus.totalCancellations, bus.totalBookedSeats, bus.totalCanceledSeats,
               bus.totalRevenue, bus.lostRevenue, bus.netRevenue);
    }

    // Print a closing line for the table
    printf("=====================================================================================================================================================\n");

    // Close the report file after reading is complete
    fclose(reportFile);
}

void filterBusReport(int filterType, char *filterValue, char comparison, float filterNumber) {
    // Open the bus report file for reading
    FILE *reportFile = fopen("bus_report.txt", "r");

    // Check if the file was opened successfully
    if (!reportFile) {
        printf("Error: Could not open bus_report.txt\n");
        return;
    }

    // Print the report header to maintain table formatting
    printReportHeader();

    char line[MAX_LINE]; // Buffer to store each line read from the file

    // Read each line from the file and parse it into a BusReservation structure
    while (fgets(line, sizeof(line), reportFile)) {
        struct BusReservation bus;

        // Extract data from the CSV format line, including new fields for booked and canceled seats
        sscanf(line, "%d,%[^,],%[^,],%d,%d,%d,%d,RM %f,RM %f,RM %f",
               &bus.busID, bus.busNumberPlate, bus.date, &bus.totalBookings,
               &bus.totalCancellations, &bus.totalBookedSeats, &bus.totalCanceledSeats,
               &bus.totalRevenue, &bus.lostRevenue, &bus.netRevenue);

        // Filter by bus ID (filterType == 1)
        if (filterType == 1 && bus.busID != atoi(filterValue)) {
            continue; // Skip this entry if it does not match
        }

        // Filter by bus number plate (filterType == 2)
        if (filterType == 2 && strcmp(bus.busNumberPlate, filterValue) != 0) {
            continue; // Skip if the number plate does not match
        }

        // Filter by date (filterType == 3)
        if (filterType == 3 && strstr(bus.date, filterValue) == NULL) {
            continue; // Skip if the date does not contain the specified value
        }

        // Filter by total bookings (filterType == 4)
        if (filterType == 4) {
            if ((comparison == 'M' && bus.totalBookings <= filterNumber) ||
                (comparison == 'L' && bus.totalBookings >= filterNumber)) {
                continue; // Skip if the condition is not met
            }
        }

        // Filter by total cancellations (filterType == 5)
        if (filterType == 5) {
            if ((comparison == 'M' && bus.totalCancellations <= filterNumber) ||
                (comparison == 'L' && bus.totalCancellations >= filterNumber)) {
                continue; // Skip if the condition is not met
            }
        }

        // Filter by total booked seats (filterType == 7)
        if (filterType == 6) {
            if ((comparison == 'M' && bus.totalBookedSeats <= filterNumber) ||
                (comparison == 'L' && bus.totalBookedSeats >= filterNumber)) {
                continue; // Skip if the condition is not met
            }
        }

        // Filter by total canceled seats (filterType == 8)
        if (filterType == 7) {
            if ((comparison == 'M' && bus.totalCanceledSeats <= filterNumber) ||
                (comparison == 'L' && bus.totalCanceledSeats >= filterNumber)) {
                continue; // Skip if the condition is not met
            }
        }

        // Filter by net revenue (filterType == 6)
        if (filterType == 8) {
            if ((comparison == 'M' && bus.netRevenue <= filterNumber) ||
                (comparison == 'L' && bus.netRevenue >= filterNumber)) {
                continue; // Skip if the condition is not met
            }
        }

        // Print the filtered bus reservation data in the table format
        printf("| %-5d | %-12s | %-10s | %-12d | %-13d | %-13d | %-14d | RM %-10.2f | RM %-10.2f | RM %-10.2f |\n",
               bus.busID, bus.busNumberPlate, bus.date, bus.totalBookings,
               bus.totalCancellations, bus.totalBookedSeats, bus.totalCanceledSeats,
               bus.totalRevenue, bus.lostRevenue, bus.netRevenue);
    }

    // Print a closing line for the table
    printf("=====================================================================================================================================================\n");

    // Close the report file after reading is complete
    fclose(reportFile);
}

void generateUserReport() {
    // Open necessary files for reading reservations and cancellations, and writing the user report
    FILE *resFile = fopen("reservation.txt", "r");
    FILE *cancelFile = fopen("cancellations.txt", "r");
    FILE *reportFile = fopen("user_report.txt", "w");

    // Check if files opened successfully
    if (!resFile || !cancelFile || !reportFile) {
        printf("Error opening files!\n");
        return;
    }

    // Arrays to store user-related data
    char usernames[MAX_USERS][50];  // Stores unique usernames
    float userSpending[MAX_USERS] = {0};  // Total amount spent per user
    float userRefunds[MAX_USERS] = {0};   // Total refund amount per user
    int userBookings[MAX_USERS] = {0};    // Number of bookings per user
    int userCancellations[MAX_USERS] = {0};  // Number of cancellations per user
    int localUserCount = 0;  // Total number of unique users found

    char username[50];  // Buffer for username extraction
    float amount;       // Buffer for amount extraction

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), resFile)) {
        // Extract username and amount paid from the reservation record
        sscanf(line, "%49[^,],%*d,%*d,%*[^,],%*[^,],%*d,%*[^,],%f", username, &amount);

        // Skip empty usernames
        if (strlen(username) == 0) continue;

        // Check if the user is already recorded
        int userIndex = -1;
        for (int i = 0; i < localUserCount; i++) {
            if (strcmp(usernames[i], username) == 0) {
                userIndex = i;
                break;
            }
        }

        // If the user is not found, add them to the list
        if (userIndex == -1) {
            userIndex = localUserCount;
            strcpy(usernames[localUserCount], username);
            localUserCount++;
        }

        // Increment booking count and add spending amount for the user
        userBookings[userIndex]++;
        userSpending[userIndex] += amount;
    }
    fclose(resFile); // Close reservation file after processing

    while (fgets(line, sizeof(line), cancelFile)) {
        // Extract username and refund amount from the cancellation record
        sscanf(line, "%49[^,],%*d,%*d,%*[^,],%*[^,],%*d,%*[^,],%f", username, &amount);

        // Skip empty usernames
        if (strlen(username) == 0) continue;

        // Find the user in the recorded list
        for (int i = 0; i < localUserCount; i++) {
            if (strcmp(usernames[i], username) == 0) {
                // Increment cancellation count and add refund amount for the user
                userCancellations[i]++;
                userRefunds[i] += amount;
                break;
            }
        }
    }
    fclose(cancelFile); // Close cancellation file after processing


    for (int i = 0; i < localUserCount; i++) {
        // Skip users who have neither bookings nor cancellations
        if (userBookings[i] == 0 && userCancellations[i] == 0) continue;

        // Calculate the average spending per booking (avoid division by zero)
        float avgSpending = (userBookings[i] > 0) ? (userSpending[i] / userBookings[i]) : 0.0;

        // Write the formatted user data to the report file
        fprintf(reportFile, "%s,%d,%d,RM %.2f,RM %.2f,RM %.2f\n",
                usernames[i], userBookings[i], userCancellations[i],
                userSpending[i], userRefunds[i], avgSpending);
    }

    fclose(reportFile); // Close the report file after writing is complete
}

void printUserReport() {
    FILE *reportFile = fopen("user_report.txt", "r");
    if (!reportFile) {
        printf("Error: Could not open user_report.txt\n");
        return;
    }

    printf("==============================================================================================\n");
    printf("| %-12s | %-10s | %-14s | %-13s | %-13s | %-13s |\n",
           "Username", "Bookings", "Cancellations", "Total Spent", "Total Refund", "Avg Spending");
    printf("==============================================================================================\n");

    char username[50];
    int totalBookings, totalCancellations;
    float totalSpent, totalRefund, avgSpending;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), reportFile)) {
        sscanf(line, "%49[^,],%d,%d,RM %f,RM %f,RM %f",
               username, &totalBookings, &totalCancellations,
               &totalSpent, &totalRefund, &avgSpending);

        printf("| %-12s | %-10d | %-14d | RM %-10.2f | RM %-10.2f | RM %-10.2f |\n",
               username, totalBookings, totalCancellations, totalSpent, totalRefund, avgSpending);
    }

    printf("==============================================================================================\n");

    fclose(reportFile);
}

// Function to print header for both reservations and cancellations
void printHeader(const char *title) {
    // Print the title of the report
    printf("\n%s\n", title);
    // Print the separator line for visual clarity
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    // Print the table headers: username, ticket number, bus ID, bus number plate, date, number of seats, seat numbers, and amount
    printf("| %-12s | %-12s | %-6s | %-15s | %-12s | %-8s | %-15s | %-10s |\n",
           "Username", "Ticket No.", "BusID", "Number Plate", "Date", "Seats", "Seat Numbers", "Amount");
    // Print the separator line for visual clarity
    printf("-------------------------------------------------------------------------------------------------------------------\n");
}

// Function to print all reservations
void printReservations() {
    // Open the reservations file for reading
    FILE *file = fopen("reservation.txt", "r");
    if (!file) {
        // If file doesn't exist or can't be opened, print an error message and return
        printf("Error: Could not open reservation.txt for reading!\n");
        return;
    }

    // Call the printHeader function to print the report title and column headers
    printHeader("Reservations Report");

    char line[256], username[50], busNumberPlate[20], bookingDate[20], seatsBuffer[100];
    int ticketNumber, busID, numSeats;
    float finalAmount;

    // Read each line of the reservation file
    while (fgets(line, sizeof(line), file)) {
        // Use sscanf to parse the line into respective fields: username, ticket number, bus ID, bus number plate, etc.
        if (sscanf(line, "%49[^,],%d,%d,%19[^,],%19[^,],%d,%99[^,],%f",
                   username, &ticketNumber, &busID, busNumberPlate, bookingDate, &numSeats, seatsBuffer, &finalAmount) == 8) {
            // Print the parsed data into a formatted table
            printf("| %-12s | %-12d | %-6d | %-15s | %-12s | %-8d | %-15s | RM%-8.2f |\n",
                   username, ticketNumber, busID, busNumberPlate, bookingDate, numSeats, seatsBuffer, finalAmount);
        }
    }

    // Print the separator line after all data has been printed
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    // Close the file after reading
    fclose(file);
}

// Function to print all cancellations
void printCancellations() {
    // Open the cancellations file for reading
    FILE *file = fopen("cancellations.txt", "r");
    if (!file) {
        // If file doesn't exist or can't be opened, print an error message and return
        printf("Error: Could not open cancellations.txt for reading!\n");
        return;
    }

    // Call the printHeader function to print the report title and column headers
    printHeader("Cancellations Report");

    char line[256], username[50], busNumberPlate[20], cancellationDate[20], seatsBuffer[100];
    int ticketNumber, busID, numSeats;
    float refundAmount;

    // Read each line of the cancellations file
    while (fgets(line, sizeof(line), file)) {
        // Use sscanf to parse the line into respective fields: username, ticket number, bus ID, bus number plate, etc.
        if (sscanf(line, "%49[^,],%d,%d,%19[^,],%19[^,],%d,%99[^,],%f",
                   username, &ticketNumber, &busID, busNumberPlate, cancellationDate, &numSeats, seatsBuffer, &refundAmount) == 8) {
            // Print the parsed data into a formatted table
            printf("| %-12s | %-12d | %-6d | %-15s | %-12s | %-8d | %-15s | RM%-8.2f |\n",
                   username, ticketNumber, busID, busNumberPlate, cancellationDate, numSeats, seatsBuffer, refundAmount);
        }
    }

    // Print the separator line after all data has been printed
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    // Close the file after reading
    fclose(file);
}

// Function to filter reservations and cancellations based on different criteria
void filterRecords(const char *filename, int filterType, const char *filterValue) {
    // Open the file (either reservation.txt or cancellations.txt) for reading
    FILE *file = fopen(filename, "r");
    if (!file) {
        // If file can't be opened, print an error message and return
        printf("Error: Could not open %s for reading!\n", filename);
        return;
    }

    // Print the header for the filtered report (filtered reservations or cancellations)
    printHeader(strcmp(filename, "reservation.txt") == 0 ? "Filtered Reservations" : "Filtered Cancellations");

    char line[256], username[50], busNumberPlate[20], date[20], seatsBuffer[100];
    int ticketNumber, busID, numSeats;
    float amount;

    // Read each line of the file
    while (fgets(line, sizeof(line), file)) {
        // Parse each line into respective fields: username, ticket number, bus ID, bus number plate, etc.
        if (sscanf(line, "%49[^,],%d,%d,%19[^,],%19[^,],%d,%99[^,],%f",
                   username, &ticketNumber, &busID, busNumberPlate, date, &numSeats, seatsBuffer, &amount) == 8) {
            // Apply the filter condition based on the filter type
            if ((filterType == 1 && strcmp(username, filterValue) != 0) ||  // Filter by username
                (filterType == 2 && busID != atoi(filterValue)) ||         // Filter by bus ID
                (filterType == 3 && strcmp(busNumberPlate, filterValue) != 0) ||  // Filter by bus number plate
                (filterType == 4 && strstr(date, filterValue) == NULL)) {   // Filter by date (substring match)
                continue;  // Skip the record if the filter doesn't match
            }
            // If the filter condition is met, print the filtered record
            printf("| %-12s | %-12d | %-6d | %-15s | %-12s | %-8d | %-15s | RM%-8.2f |\n",
                   username, ticketNumber, busID, busNumberPlate, date, numSeats, seatsBuffer, amount);
        }
    }

    // Print the separator line after the filtered data has been printed
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    // Close the file after reading
    fclose(file);
}

// Function to display report options and handle user selection
void viewReport() {
    int option;
    do {
        // Print the menu options for viewing reports
        printf("\nView Report:\n");
        printf("1. Full Bus Report\n");
        printf("2. Filter Bus Report\n");
        printf("3. User Report\n");
        printf("4. Reservations Report\n");
        printf("5. Filter Reservations Report\n");
        printf("6. Cancellations Report\n");
        printf("7. Filter Cancellations Report\n");
        printf("8. Back to Admin Menu\n");
        printf("Enter your choice: ");

        // Read user input for menu selection
        if (scanf("%d", &option) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer to avoid invalid input issues
            continue;
        }

        // Switch case to handle different menu options
        if (option == 1) {
            printBusReport(); // This function should be defined to print a full bus report
        } else if (option == 2) {
            // Handle filtering for bus report by different criteria (bus ID, plate, date, etc.)
            int filterType;
            char filterValue[50];
            char comparison;
            float filterNumber;

            do {
                // Print the filter options for filtering bus report
                printf("\nFilter Options:\n");
                printf("1. Bus ID\n");
                printf("2. Bus Number Plate\n");
                printf("3. Date (Year-Month)\n");
                printf("4. Total Bookings\n");
                printf("5. Total Cancellations\n");
                printf("6. Total Booked Seats\n");
                printf("7. Total Canceled Seats\n");
                printf("8. Net Revenue\n");
                printf("9. Return to View Report\n");
                printf("Enter Choice: ");
                if (scanf("%d", &filterType) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    while (getchar() != '\n'); // Clear input buffer
                    continue;
                }

                if (filterType == 9) break;  // Exit the loop if the user selects 'Return to View Report'

                // Handle filter conditions based on the selected filter type
                if (filterType >= 1 && filterType <= 3) {
                    printf("Enter value to filter by: ");
                    scanf("%s", filterValue);
                    filterBusReport(filterType, filterValue, '\0', 0);
                } else if (filterType >= 4 && filterType <= 8) {
                    printf("Enter comparison type ('M' for More/'L' for Less): ");
                    scanf(" %c", &comparison);
                    comparison = toupper(comparison);  // Ensure comparison is uppercase for consistency
                    if (comparison != 'M' && comparison != 'L') {
                        printf("Invalid comparison type! Use 'M' for more or 'L' for less.\n");
                        continue;
                    }

                    printf("Enter number: ");
                    if (scanf("%f", &filterNumber) != 1) {
                        printf("Invalid input! Please enter a valid number.\n");
                        while (getchar() != '\n'); // Clear input buffer
                        continue;
                    }
                    filterBusReport(filterType, "", comparison, filterNumber);
                } else {
                    printf("Invalid filter type!\n");
                }
            } while (1);
        } else if (option == 3) {
            printUserReport(); // Display user report
        } else if (option == 4) {
            printReservations(); // Display full reservations report
        } else if (option == 5) {
            int filterType;
            char filterValue[50];
            // Handle filtering reservations by various criteria
            printf("\nFilter Reservations Report:\n");
            printf("1. Filter by Username\n");
            printf("2. Filter by Bus ID\n");
            printf("3. Filter by Bus Number Plate\n");
            printf("4. Filter by Date\n");
            printf("Enter filter option: ");
            if (scanf("%d", &filterType) != 1) {
                printf("Invalid input! Please enter a number.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            // Read the value to filter by
            printf("Enter value to filter by: ");
            scanf("%s", filterValue);
            filterRecords("reservation.txt", filterType, filterValue); // Filter and display reservations
        } else if (option == 6) {
            printCancellations(); // Display full cancellations report
        } else if (option == 7) {
            int filterType;
            char filterValue[50];
            // Handle filtering cancellations by various criteria
            printf("\nFilter Cancellations Report:\n");
            printf("1. Filter by Username\n");
            printf("2. Filter by Bus ID\n");
            printf("3. Filter by Bus Number Plate\n");
            printf("4. Filter by Date\n");
            printf("Enter filter option: ");
            if (scanf("%d", &filterType) != 1) {
                printf("Invalid input! Please enter a number.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            // Read the value to filter by
            printf("Enter value to filter by: ");
            scanf("%s", filterValue);
            filterRecords("cancellations.txt", filterType, filterValue); // Filter and display cancellations
        } else if (option != 8) {
            // If an invalid option is entered, prompt the user to try again
            printf("Invalid option! Please try again.\n");
        }
    } while (option != 8); // Exit the loop when the user selects the "Back to Admin Menu" option
}

int main() {
    srand(time(NULL)); // Seed random number generator for unique ticket numbers

    struct BusReservation buses[MAX_BUSES]; // Array to store bus schedules
    int busCount = loadBuses(buses, MAX_BUSES); // Load bus data from file
    int resCount = loadSeats(buses, MAX_BUSES); // Load seat reservation data
    loadUsers(); // Load registered users into memory

    struct user currentUser; // Stores the currently logged-in user
    int choice; // Stores menu choice input
    int loggedInAsAdmin = 0; // Flag to track if admin is logged in
    int loggedInAsUser = 0; // Flag to track if a user is logged in

    while (1) { // Main system loop, runs until exit
        // Display the main menu
        printf("\nBus Reservation System\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Register\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice); // Get user input

        switch (choice) {
            case 1: // Admin login
                if (loginAdmin()) { // If login successful
                    loggedInAsAdmin = 1;
                }
                break;
            case 2: // User login
                currentUser = loginUser(); // Authenticate user
                if (strlen(currentUser.username) > 0) { // Check if login succeeded
                    loggedInAsUser = 1;
                }
                break;
            case 3: // User registration
                registerUser();
                break;
            case 4: // Exit program
                printf("Exiting the system...\n");
                return 0;
            default: // Handle invalid input
                printf("Invalid choice! Please try again.\n");
                continue; // Restart loop
        }

        // Admin Menu (Only accessible if logged in as admin)
        while (loggedInAsAdmin) {
            printf("\nAdmin Menu\n");
            printf("1. Check Bus Status\n");
            printf("2. View Availability\n");
            printf("3. Add Bus Schedule\n");
            printf("4. Update Bus Schedule\n");
            printf("5. Delete Bus Schedule\n");
            printf("6. View Report\n");
            printf("7. Logout\n");
            printf("Enter your choice: ");

            scanf("%d", &choice); // Get admin menu choice

            switch (choice) {
                case 1:
                    checkBusStatus(buses, busCount);
                    break;
                case 2:
                    viewAvailability(buses, busCount);
                    break;
                case 3:
                    addBusSchedule(buses, &busCount, MAX_BUSES);
                    break;
                case 4:
                    updateBusSchedule(buses, &busCount, currentUser);
                    break;
                case 5:
                    deleteBusSchedule(buses, &busCount);
                    break;
                case 6:
                    generateReports(buses, busCount); // Generate reports before viewing
                    viewReport(); // View various reports
                    break;
                case 7:
                    printf("Logging out...\n");
                    loggedInAsAdmin = 0; // Exit admin menu
                    break;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        }

        // User Menu (Only accessible if logged in as a user)
        while (loggedInAsUser) {
            printf("\nUser Menu\n");
            printf("1. Check Bus Status\n");
            printf("2. Search Buses\n");
            printf("3. View Availability\n");
            printf("4. Book Seats\n");
            printf("5. View Notifications\n");
            printf("6. View Ticket\n");
            printf("7. Cancel Booking\n");
            printf("8. Update User Information\n");
            printf("9. View Booking History\n");
            printf("10. Logout\n");
            printf("Enter your choice: ");

            scanf("%d", &choice); // Get user menu choice

            switch (choice) {
                case 1:
                    checkBusStatus(buses, busCount);
                    break;
                case 2:
                    searchBuses(buses, busCount);
                    break;
                case 3:
                    viewAvailability(buses, busCount);
                    break;
                case 4: // Booking process
                    processBooking(currentUser, buses, &busCount);
                    break;
                case 5:
                    viewNotifications(&currentUser);
                    break;
                case 6: { // View ticket details
                    int ticketCount = loadTicketNumbers(buses, busCount); // Load ticket data
                    getTicketDetails(buses, busCount); // Retrieve ticket info
                    break;
                }
                case 7:
                    cancelBooking(currentUser, buses, busCount);
                    break;
                case 8:
                    updateUser();
                    break;
                case 9:
                    viewBookingHistory(currentUser);
                    break;
                case 10:
                    printf("Logging out...\n");
                    loggedInAsUser = 0; // Exit user menu
                    break;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        }
    }

    return 0;
}
