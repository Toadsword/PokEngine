#include <gtest/gtest.h>

#include <xxhash.h>

TEST(HASH, TestHash) {
		// Initialize state with selected seed 
	XXH64_hash_t const seed = 0;   // or any other value 

	const char* str1 = "Test";
	const char* str2 = "Test";

	XXH64_hash_t hash1 = XXH64(str1, 4, seed);
	std::cout << "hash of \"Test\" == " << hash1 << "\n";
	XXH64_hash_t hash2 = XXH64(str2, 4, seed);
	std::cout << "hash of \"Test\" == " << hash2 << "\n";

    EXPECT_EQ(hash1, hash2);

	std::string filePathString = "../AppData/Textures/Metals.jpg";
	const char* filePath = "../AppData/Textures/Metals.jpg";
	XXH64_hash_t hashPath = XXH64(filePath, filePathString.size(), seed);

	std::cout << "Hash(" << filePath << ") = " << hashPath << "\n";
	EXPECT_NE(hashPath, hash2);
}