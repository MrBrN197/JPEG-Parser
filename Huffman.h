

#include "core.h"

#include <stdio.h>


#define BREAK ASSERT(false)

struct Table {
	Table* left = nullptr;
	Table* right = nullptr;
	void insert(int x) {
		BREAK
	}
	int length() const{
		BREAK
		return NULL;
	}
	Table* operator[](int idx) {
		BREAK
		return nullptr;
	}
};


bool InsertValue(Table* root, u32 element, u8 bits){

	if (bits == 1){
		ASSERT(root->length() <= 2);
		if (root->length() == 2)
			return false;
		root->insert(element);
		return true;
	}

	if(!root->length()){
		BREAK
	}
	// insert left (0)
	if (InsertValue(root->left, element, bits-1)){
		return true;
	}
	if (root->length() == 1){
		BREAK
	}
	// try insert right (1)
	return InsertValue(root->right, element, bits-1);
}


Table* CreateHuffmanTable(u32 lengths[16], u8* elements) {
	Table* root = nullptr;
	u32 idx = 0;

	for(u8 bits = 1; bits <= 16; bits++) {
		printf("Bits: %d -> ", bits);
		for(u32 e = 0; e < lengths[bits-1]; e++){
			printf(" %d ", (u32)elements[idx]);
			InsertValue(root, elements[idx], bits);
			idx++;
		}
		printf("\n");
	}

	return root;
}


