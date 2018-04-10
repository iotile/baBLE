#include <iostream>
#include <memory>
#include <uvw.hpp>
#include "Log.hpp"
#include "utils/colors.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include "utils/stream_formats.hpp"

using namespace std;
using namespace uvw;

int main() {
  ENABLE_LOGGING(DEBUG);

  uint8_t test0 = 15;
  uint8_t test1 = 10;
  string str;
  str += "test";
  array<uint8_t, 5> truc = {2, 8, 12, 15, 55};
  vector<uint8_t> bidule = {1, 3, 88, 155};
  Serializer ser;
  ser << test0 << test1 << truc << bidule << str;
  cout << ser;

  const uint8_t* buffer = ser.buffer();
  for(int i = 0; i < ser.size(); i++) {
    printf("%d / ", buffer[i]);
  }
  cout << endl;

  uint8_t test2;
  uint8_t test3;
  array<uint8_t, 5> truc2{};
  vector<uint8_t> bidule2(4);
  string str2;
  Deserializer deser(buffer, ser.size());
  cout << deser;
  deser >> test2 >> test3 >> truc2 >> bidule2 >> str2;
  cout << HEX(test2) << " " << HEX(test3) << endl;
  for(const uint8_t& v : truc2) {
    cout << HEX(v) << " / ";
  }
  cout << endl;

  for(const uint8_t& v : bidule2) {
    cout << HEX(v) << " / ";
  }
  cout << endl;
  cout << str2 << endl;

//  Loop loop = Loop::getDefault();
//  std::cout << COLORIZE(RED_COLOR, "OK") << std::endl;
//
//  TimerHandle timer = loop->resource<TimerHandle>();
//  int counter = 0;
//  timer->on<TimerEvent>([&counter](const TimerEvent&, TimerHandle& t) {
//    counter ++;
//    LOG.debug("TIC #" + to_string(counter), "Timer");
//    if (counter >= 5) {
//      t.loop().stop();
//    }
//  });
//  timer->start(chrono::seconds(1), chrono::seconds(2));
//
//  string test[2];
//  cin >> test[0] >> test[1];
//  cout << test[0];
//  cout << test[1];
//  cout << "test";
//
//  loop->run<Loop::Mode::DEFAULT>();
  printf("END");
  return 0;
}