#include <gtest/gtest.h>

#include <Utility/json_utility.h>

TEST(JSON, JsonEqual) {

	json firstJson;
	firstJson["table"][0]["coucou"] = "three";
	firstJson["table"][1]["number"] = 3;

	json secondJson;
	secondJson["table"][0]["coucou"] = "three";
	secondJson["table"][1]["number"] = 3;

	EXPECT_EQ(firstJson, secondJson);
}