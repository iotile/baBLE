#include <iostream>
#include <memory>
#include <uvw.hpp>
#include "Log.h"
#include "utils/colors.h"

using namespace std;
using namespace uvw;

int main() {
  ENABLE_LOGGING(DEBUG);

  auto loop = Loop::getDefault();
  std::cout << "OK" << std::endl;

  auto timer = loop->resource<TimerHandle>();
  int counter = 0;
  timer->on<TimerEvent>([&counter](const TimerEvent&, TimerHandle& t) {
    counter ++;
    printf("TIC #%d\n", counter);
    if (counter >= 5) {
      t.loop().stop();
    }
  });
  timer->start(chrono::seconds(5), chrono::seconds(2));

  loop->run<Loop::Mode::DEFAULT>();
  printf("END");
  return 0;
}