namespace input{ //Non-Blocking Console Input

  std::thread iothread;
  std::mutex mutex;
  std::condition_variable cv;
  std::deque<std::string> lines;
  std::deque<std::string> process;

  void hide(bool h = true){
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( h ) tty.c_lflag &= ~ECHO;
    else    tty.c_lflag |=  ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
  }

  std::string forbidden = "\\^[\"";
  void sanitize(std::string& dirty){
    std::string clean;
    for(auto& s: dirty){
      if(forbidden.find(s) != std::string::npos)
        clean.push_back('\\');
      clean.push_back(s);
    }
    dirty = clean;
  }

  void start(){
    iothread = std::thread([&](){
      std::string tmp;
      while (true) {
        std::getline(std::cin, tmp);
        sanitize(tmp);
        std::lock_guard lock{mutex};
        lines.push_front(std::move(tmp));
        cv.notify_one();
      }
    });
  }

  bool read(std::string& s){
    { //Critical
      std::unique_lock lock{mutex};
      if (cv.wait_for(lock, std::chrono::seconds(0), [&]{ return !lines.empty(); }))
        std::swap(lines, process);
    }

    if (!process.empty()) {
      s = process.back();
      process.pop_back();
      return true;
    }
    return false;
  }

};
