#include "texted.hpp"

int main( int argc , char **argv ){
  if(argc > 1){
    auto texted = std::make_shared<TextEd>(argv[1]);
    texted->run();
  }else{
    auto texted = std::make_shared<TextEd>("");
    texted->run();
  }
  //echo();
  return 0;
}
