

#include "core.h"

#include <stdio.h>


#define BREAK ASSERT(false)

struct Table {
	union {
		// if isValue then right is nullptr and left has value 
		Table* left = nullptr;		// NOTE: use a u16 offset pointer from base instead
		u64 value;
	};
	Table* right = nullptr;
	bool isValue = false;
	void create_table(u8 pos) {
		ASSERT(pos == 0 || pos == 1)
		if(pos == 0){
			this->left = new Table;
		}else {
			this->right = new Table;
		}
	}
	void set_value(u32 element) {
		this->value = element;
		isValue = true;
	}
	bool has_children() {
		return this->left || this->right;
	}
	u8 length() const{
		u8 length = 0;
		if(this->left)
			length++;
		if(this->right)
			length++;
		return length;
	}
	Table* operator[](int idx) {
		BREAK
		return nullptr;
	}
};


bool InsertValue(Table* root, u32 element, u8 bits){
	if (root->isValue){
		return false;
	}

	if (bits == 0){
		ASSERT(root->length() <= 2);
		if (root->has_children())
			return false;	// can't add element if two children are already tables
		root->set_value(element);
		return true;
	}

	if(!root->length()){
		root->create_table(0);	// create left table (0)
	}
	// try insert left (0)
	if (InsertValue(root->left, element, bits-1)){
		return true;
	}
	if (root->length() == 1){
		root->create_table(1);	// create right table (1)
	}
	// try insert right (1)
	return InsertValue(root->right, element, bits-1);
}


Table* CreateHuffmanTable(u32 lengths[16], u8* elements) {
	Table* root = new Table;
	u32 idx = 0;

	for(u8 bits = 1; bits <= 16; bits++) {
		//printf("Bits: %d -> ", bits);
		for(u32 e = 0; e < lengths[bits-1]; e++){
			//printf(" %d ", (u32)elements[idx]);
			InsertValue(root, elements[idx], bits);
			idx++;
		}
		//printf("\n");
	}

	return root;
}


