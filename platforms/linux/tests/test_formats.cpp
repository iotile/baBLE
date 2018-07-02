#include <catch.hpp>
#include <uv.h>
#include <Format/HCI/HCIFormat.hpp>
#include <Transport/Socket/HCI/HCISocket.hpp>


int Factorial( int number ) {
//  uv_loop_t* loop = uv_default_loop();
//  uv_loop_init(loop);
//
//  std::shared_ptr<HCIFormat> hci_format = std::make_shared<HCIFormat>();
//  HCISocket hci_socket(loop, hci_format, 0);

//  return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail


//  FILE* output = nullptr;
//  output = freopen("output.log","w", stdout);
//  if (output == nullptr) {
//    printf("ERROR OUTPUT");
//  }
//
//  fclose(output);

  return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE("Factorial of 0 is 1 (fail)", "[single-file]") {
  REQUIRE(Factorial(0) == 1);
}

TEST_CASE("Factorials of 1 and higher are computed (pass)", "[single-file]") {
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}
