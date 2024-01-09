#include "texted.hpp"

TextEd::TextEd(const std::string& file){

  x = y = 0;
  mode = 'n';
  status = " VIEW";
  section = {};

  if(file.empty()){
    filename =  "untitled";
  }else{
    filename = file;
  }

  open();

  initscr();
  noecho();
  cbreak();
  keypad(stdscr, true);
  use_default_colors();
  start_color();
}

TextEd::~TextEd(){
  refresh();
  endwin();
}

void TextEd::run(){
	//printw("press i to go to insert mode, q to quit. In insert mode, press escape to go back to view mode.\n");
	//refresh();
  while(mode != 'q'){
    update(); 
    statusline();
    print();
    int c = getch();
    input(c);
  }
}

void TextEd::update(){
  switch (mode){
    case 'n':
      status = " VIEW";
      break;
    case 'i':
      status = " INSERT";
      break;
    case 'q':
      break;
  }

  section = " COLS: " + std::to_string(x) + " | ROWS: " + std::to_string(y) + " | FILE: " + filename;
  //status.insert(0, " ");
}

void TextEd::statusline(){

  //start_color();
  if(mode == 'n'){
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
  }else{
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
  }

  attron(A_REVERSE);
  attron(A_BOLD);
  attron(COLOR_PAIR(1));
  for (int i =0; i < COLS; i++) {
   mvprintw(LINES - 1, i, " "); 
  }
  mvprintw(LINES - 1, 0, status.c_str());
  mvprintw(LINES - 1, COLS - section.length(), section.c_str());
  attroff(COLOR_PAIR(1));
  attroff(A_BOLD);
  attroff(A_REVERSE);
}

void TextEd::input(int c){

  switch (c){
    case KEY_UP:
      up(); 
      return;
    case KEY_LEFT:
      left(); 
      return;
    case KEY_RIGHT:
      right(); 
      return;
    case KEY_DOWN:
      down(); 
      return;
  }

  switch (mode){
    case 27:
    	//printw("press:\ni for insert mode\nn for normal mode\nw to save the file\n q to quit\n");
    	//refresh();
    case 'n':
      switch (c){
        case 'q':
          mode = 'q';
         break;
        case 'i':
          mode = 'i';
          break;
        case 'w':
          mode = 'w';
          save();
          refresh();
          endwin();
          std::printf("\nSaved!\n");
          echo();
          nocbreak();
          system("stty echo");
          system("reset");
          system("clear");
          system("echo 'saved!'");
          exit(0);
          // TODO save withot exit
          break;
      }
    break;
    case 'i':
      switch (c){
        case 27:
          mode = 'n';
          //printw("press:\ni for insert mode\nn for normal mode\nq to quit\n");
          //refresh();
          break;
        case 127:
        case KEY_BACKSPACE:
          if( x == 0 && y > 0){
            x = lines[y - 1].length();
            lines[y - 1] += lines[y];
            m_remove(y);
            up();
          }else if( x > 0){
            lines[y].erase(--x, 1);
            
          }
        break;
        case KEY_DC:
          if( x == lines[y].length() && y != lines.size() - 1){
            lines[y] += lines[y + 1];
          }else{
            lines[y].erase(x, 1);
          }
        break;
        case KEY_ENTER:
        case 10:
         if( x < lines[y].length()){
           m_insert(lines[y].substr(x, lines[y].length() - x), y + 1);
           lines[y].erase(x, lines[y].length() - x);
         }else{
           m_insert("", y + 1);
         }
         x = 0;
         down();
        break;
        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
          lines[y].insert(x, 2, ' ');
          x += 2;
        break;
        default:
          lines[y].insert(x, 1, c);
          ++x;
      }
  }
}

void TextEd::print(){
  for (size_t i =0; i < (size_t)LINES - 1; i++) {
    if(i >= lines.size()){
      move(i, 0);
      clrtoeol();
    }else{
      mvprintw(i, 0, lines[i].c_str());
    }
    clrtoeol();
  }
  move(y, x);
}

void TextEd::m_remove(int number){
  lines.erase(lines.begin() + number);
}

std::string TextEd::m_tabs(std::string& line){
  std::size_t tab = line.find('\t');
  return tab == line.npos ? line : m_tabs(line.replace(tab, 1, "  "));
}

void TextEd::m_insert(std::string line, int number){
  line = m_tabs(line);
  lines.insert(lines.begin() + number, line);
}

void TextEd::m_append(std::string& line){
  line = m_tabs(line);
  lines.push_back(line);
}

void TextEd::up(){
  if(y > 0){
    --y;
  }
  if(x >= lines[y].length()){
    x = lines[y].length();
  }
  move(y, x);
}

void TextEd::left(){
  if(x > 0){
    --x;
    move(y, x);
  }
}

void TextEd::right(){
  if( (int)x <= COLS && x <= lines[y].length() - 1){
    ++x;
    move(y, x);
  }
}

void TextEd::down(){
  if( (int)y < LINES && y < lines.size() - 1){
    ++y;
  }

  if( x >= lines[y].length()){
    x = lines[y].length();
  }
  move(y, x);
}

void TextEd::open(){
  if(std::filesystem::exists(filename)){
    std::ifstream ifile(filename);
    if(ifile.is_open()){
      while(!ifile.eof()){
        std::string buffer;
        std::getline(ifile, buffer);
        m_append(buffer);
      }
    }else{
      throw std::runtime_error("Cannot open file. Permission denied.");
    }
  }else{
    std::string str {};
    m_append(str);
  }
}

void TextEd::save(){
  std::ofstream ofile(filename);
  if(ofile.is_open()){
    for (size_t i =0; i < lines.size();i++) {
     ofile << lines[i] << '\n'; // CHECK i is last
    }
    ofile.close();
  }else{
    refresh();
    endwin();
    throw std::runtime_error("Cannot save file. Permission denied.");
  }
}
