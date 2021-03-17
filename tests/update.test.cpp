//
// Created by Markus Le Roux on 2021-03-17.
//

#include "../include/Catch2/catch.hpp"
#include "TestClasses.h"


TEST_CASE("update class", "[Update]") {
	ModelTest mdl(5, 7, 3, 0.99, "cycle");
	SECTION("methods") {
		SECTION("sample from bounding list") {
			BoundingList bl(7);
			bl.set(1), bl.set(3), bl.set(5);
			for (int i = 0; i < 10; i++) {
				CHECK(std::set{1,3,5}.count(UpdateTest::bs_uniformSample_p(bl)) == 1);
			}
		}

		SECTION("compute weights") {
			std::vector<int> counts{0,1,2, 3};
			std::vector<long double> result{1, 0.5, 0.25, 0.125};
			CHECK(UpdateTest::computeWeights_p(0.5, counts) == result);
		}
	}
}

TEST_CASE("compress class", "[Compress]") {
	ModelTest mdl(5, 7, 3, 0.99, "cycle");
	SECTION("methods") {
		BoundingList bl1(7);
		bl1.set(0), bl1.set(1), bl1.set(2);
		CompressTest comU1(mdl, 3, bl1);

		SECTION("update bounding chain") {
			comU1.updateBoundingChain();
			bl1.set(comU1.getC1());
			REQUIRE(mdl.getBoundingList(3) == bl1);
		}

		SECTION("update colouring") {
			comU1.updateBoundingChain();
			bl1.set(comU1.getC1());

			comU1.updateColouring();
			REQUIRE(std::set<int>{0,1,2,comU1.getC1()}.count(mdl.getColour(3)) == 1);
		}

		SECTION("update") {
			BoundingList bl2(7);
			bl2.set(0), bl2.set(1), bl2.set(2);
			REQUIRE_NOTHROW(comU1.update());
		}
	}
}

TEST_CASE("contract class", "[Contract]") {
	SECTION("methods") {
		ModelTest mdl(5, 7, 3, 0.99, "cycle");
		BoundingList A0(7);
		A0.set(0), A0.set(1), A0.set(2);

		CompressTest comU1(mdl, 1, A0);
		comU1.update();

		CompressTest comU4(mdl, 4, A0);
		comU4.update();

		ContractTest conU(mdl, 0);

		SECTION("update bounding chain") {
			conU.updateBoundingChain();
			BoundingList bl(7);
			bl.set(conU.getC1()), bl.set(conU.getC2());
			REQUIRE(mdl.getBoundingList(0) <= bl);
		}

		SECTION("update colouring") {
			conU.updateBoundingChain();
			conU.updateColouring();
			REQUIRE(std::set<int>{conU.getC1(), conU.getC2()}.count(mdl.getColour(0)) == 1);
		}

		SECTION("update") {
			REQUIRE_NOTHROW(conU.update());
		}
	}
}