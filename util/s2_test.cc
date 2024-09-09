#include "gtest/gtest.h"
#include "s2/s2cell.h"
#include "s2/s2cell_id.h"
#include "s2/s2latlng.h"
#include "s2/s2point.h"

TEST(S2CellId, FromToken) {
  S2CellId cell_id = S2CellId::FromToken("89c25c");
  EXPECT_EQ(cell_id.id(), 0x89c25c0000000000);
}

TEST(S2CellId, ToToken) {
  S2CellId cell_id(0x89c25c0000000000);
  EXPECT_EQ(cell_id.ToToken(), "89c25c");
}

TEST(S2CellId, EncodeDecode) {
  S2CellId original_cell_id(0x89c25c0000000000);
  std::string encoded = original_cell_id.ToToken();
  S2CellId decoded_cell_id = S2CellId::FromToken(encoded);
  EXPECT_EQ(original_cell_id.id(), decoded_cell_id.id());
}

TEST(S2CellId, InvalidToken) {
  S2CellId cell_id = S2CellId::FromToken("invalid");
  EXPECT_EQ(cell_id.id(), 0);
}

TEST(S2CellId, FromLatLng) {
  S2LatLng lat_lng = S2LatLng::FromDegrees(37.7749, -122.4194); // San Francisco coordinates
  S2Point point(lat_lng);
  // Construct a leaf cell containing the given point "p".
  S2CellId cell_id(point);
  // Extract the uint64 representation of the cell id.
  auto id = cell_id.id();
  EXPECT_EQ(id, 0x8085809e8e8d8c61);
}

TEST(S2CellId, ToLatLng) {
  S2CellId cell_id(0x8085809e8e8d8c61);
  S2LatLng lat_lng = cell_id.ToLatLng();
  R2Point point = lat_lng.coords();
  EXPECT_NEAR(point.x(), 0.6592963796, 1e-9);
  EXPECT_NEAR(point.y(), -2.1366215983, 1e-9);
}
