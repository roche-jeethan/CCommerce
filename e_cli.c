#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_PRODUCTS 100
#define INR_CONVERSION_RATE 84.83
#define GST_RATE 0.05
// Structure for each product in the linked list
typedef struct Product {
    int id;
    char name[50];
    float price; //$ price
    int stock;
    struct Product* next;
} Product;
typedef struct OrderNode {
    int productId;
    int quantity;
    float totalAmount; // INR total amount
    char deliveryStatus[20];
    struct OrderNode* next;
} OrderNode;

Product* productCatalog = NULL;
OrderNode* orderQueueFront = NULL;
OrderNode* orderQueueRear = NULL;

void addProduct(int id, char name[], float price, int stock) {
    Product* newProduct = (Product*)malloc(sizeof(Product));
    newProduct->id = id;
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->stock = stock;
    newProduct->next = productCatalog;
    productCatalog = newProduct;
}

void displayProducts() {
    Product* current = productCatalog;
    printf("Product Catalog (Prices in INR):\n");
    while (current != NULL) {
        printf("ID: %d, Name: %s, Price: %.2f INR, Stock: %d\n",
               current->id, current->name, current->price * INR_CONVERSION_RATE, current->stock);
        current = current->next;
    }
}

void enqueueOrder(int productId, int quantity) {
    Product* product = productCatalog;
    while (product != NULL && product->id != productId) {
        product = product->next;
    }
    if (product == NULL) {
        printf("Product ID %d not found.\n", productId);
        return;
    }
    if (quantity > product->stock) {
        printf("Error: Requested quantity (%d) exceeds available stock (%d) for product %s.\n",
               quantity, product->stock, product->name);
        return;
    }
    float totalAmount = product->price * quantity * INR_CONVERSION_RATE;
    OrderNode* newOrder = (OrderNode*)malloc(sizeof(OrderNode));
    newOrder->productId = productId;
    newOrder->quantity = quantity;
    newOrder->totalAmount = totalAmount;
    strcpy(newOrder->deliveryStatus, "Pending"); // Initial status is only "Pending"
    newOrder->next = NULL;
    if (orderQueueRear == NULL) {
        orderQueueFront = orderQueueRear = newOrder;
    } else {
        orderQueueRear->next = newOrder;
        orderQueueRear = newOrder;
    }
    product->stock -= quantity;
    printf("Order placed for Product ID %d (%s), Quantity: %d, Total Amount: %.2f INR\n",
           productId, product->name, quantity, totalAmount);
}

void updateDeliveryStatus(OrderNode* order) {
    strcpy(order->deliveryStatus, "Delivered");
}

void dequeueOrder() {
    if (orderQueueFront == NULL) {
        printf("No orders in queue.\n");
        return;
    }
    OrderNode* temp = orderQueueFront;
    printf("Processing order for Product ID %d, Quantity %d, Total Amount %.2f INR, Delivery Status: %s\n",
           temp->productId, temp->quantity, temp->totalAmount, temp->deliveryStatus);
    if (strcmp(temp->deliveryStatus, "Pending") == 0) {
        strcpy(temp->deliveryStatus, "Dispatched");
    } else if (strcmp(temp->deliveryStatus, "Dispatched") == 0) {
        strcpy(temp->deliveryStatus, "Delivered");
    }
    if (strcmp(temp->deliveryStatus, "Delivered") == 0) {
        orderQueueFront = orderQueueFront->next;
        if (orderQueueFront == NULL) orderQueueRear = NULL;
        free(temp);
    }
}

void displayTotalBill() {
    if (orderQueueFront == NULL) {
        printf("Order queue is empty.\n");
        return;
    }
    OrderNode* temp = orderQueueFront;
    float grandTotal = 0.0;
    int totalItems = 0;
    printf("Order Queue with Split-Up Amounts (INR):\n");
    while (temp != NULL) {
        printf("Product ID: %d, Quantity: %d, Total Amount: %.2f, Delivery Status: %s\n",
               temp->productId, temp->quantity, temp->totalAmount, temp->deliveryStatus);
        grandTotal += temp->totalAmount;
        totalItems += temp->quantity;
        temp = temp->next;
    }
    float gstAmount = grandTotal * GST_RATE;
    float finalAmount = grandTotal + gstAmount;

    printf("\nTotal Items: %d\n", totalItems);
    printf("Total Amount : %.2f INR\n", grandTotal);
    printf("GST (5%%): %.2f INR\n", gstAmount);
    printf("Total Amount : %.2f INR\n", finalAmount);
}

// Main Menu
int main() {
    srand(time(0));
    addProduct(1, "Laptop", 499.99, 10);   
    addProduct(2, "Headphones", 29.99, 50);
    addProduct(3, "Keyboard", 19.99, 20);

    int choice, productId, quantity;
    while (1) {
        printf("\n1. Display Products\n2. Add Product to Order\n3. View Orders\n4. Process Next Order\n5. Display Total Bill\n6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                printf("Enter product ID to add to order: ");
                scanf("%d", &productId);
                printf("Enter quantity: ");
                scanf("%d", &quantity);
                enqueueOrder(productId, quantity);
                break;
            case 3:
                if (orderQueueFront != NULL) {
                    OrderNode* temp = orderQueueFront;
                    printf("Order Queue:\n");
                    while (temp != NULL) {
                        printf("Product ID: %d, Quantity: %d, Total Amount: %.2f INR, Delivery Status: %s\n",
                               temp->productId, temp->quantity, temp->totalAmount, temp->deliveryStatus);
                        temp = temp->next;
                    }
                } else {
                    printf("Order queue is empty.\n");
                }
                break;
            case 4:
                dequeueOrder();
                break;
            case 5:
                displayTotalBill();
                break;
            case 6:
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
