#ifndef TUNAMCTS_UCI_H
#define TUNAMCTS_UCI_H
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <chrono>

#include "root.h"
#include "search.h"

enum SearchState { IDLE=0, SEARCHING=1 };

inline std::atomic<bool> running = true;
inline std::atomic<SearchState> search_state = IDLE;

// time controls
inline std::atomic<int> wtime = 0, btime = 0;
inline std::atomic<int> winc = 0, binc = 0;
inline std::atomic<int> movetime = 0;
inline std::atomic<int> time_limit = 0;

inline std::chrono::high_resolution_clock::time_point start_time;
inline chess::Board pos;

// ---------------- INPUT ----------------
inline void watch_input() {
    std::string line;
    while (running && std::getline(std::cin, line)) {
        try {
            if (line == "quit") {
                running = false;
            }
            else if (line == "uci") {
                std::cout << "id name TunaMCTS\nid author BryanEquinn\nuciok\n" << std::flush;
            }
            else if (line == "isready") {
                std::cout << "readyok\n" << std::flush;
            }
            else if (line == "ucinewgame") {
                game_tree.clear();
            }
            else if (line.starts_with("position")) {
                std::vector<std::string> words;
                std::stringstream ss(line);
                std::string w;
                while (ss >> w) words.push_back(w);
                if (words[1] == "startpos") {
                    pos.setFen(chess::constants::STARTPOS);
                } else if (words[1] == "fen") {
                    std::string fen;
                    size_t i = 2;
                    while (i < words.size() && words[i] != "moves") {
                        fen += words[i] + " ";
                        i++;
                    }
                    pos.setFen(fen);
                }

                auto it = std::ranges::find(words, "moves");
                if (it != words.end()) {
                    for (++it; it != words.end(); ++it) {
                        pos.makeMove(chess::uci::uciToMove(pos,*it));
                    }
                }
            }
            else if (line.starts_with("go")) {
                std::vector<std::string> words;
                std::stringstream ss(line);
                std::string w;
                while (ss >> w) words.push_back(w);

                movetime = 0;

                for (size_t i = 1; i + 1 < words.size(); i++) {
                    if (words[i] == "wtime") wtime = std::stoi(words[i+1]);
                    else if (words[i] == "btime") btime = std::stoi(words[i+1]);
                    else if (words[i] == "winc") winc = std::stoi(words[i+1]);
                    else if (words[i] == "binc") binc = std::stoi(words[i+1]);
                    else if (words[i] == "movetime") movetime = std::stoi(words[i+1]);
                }

                start_time = std::chrono::high_resolution_clock::now();
                search_state = SEARCHING;
            }
            else if (line == "show") {
                std::cout << pos << std::flush;
            }
        }
        catch (...) {}
    }
}

// ---------------- MAIN LOOP ----------------
inline void uci() {
    std::thread input_thread(watch_input);
    while (running) {
        if (search_state == SEARCHING) {
            // time management
            if (movetime == 0) {
                int time_left = (pos.sideToMove() == chess::Color::WHITE) ? wtime.load() : btime.load();
                int inc = (pos.sideToMove() == chess::Color::WHITE) ? winc.load() : binc.load();
                time_limit = (time_left / 20) + (inc / 2);
            } else {
                time_limit = movetime.load();
            }
            Engine_return ret;
            auto p=pos;
            ret = root(p, time_limit);
            const int elapsed = std::max(1, static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                       std::chrono::high_resolution_clock::now() - start_time).count()));
            std::cout << "info score % "
                  << ret.value
                  << " nodes " << ret.visits
                  << " nps " << (ret.visits * 1000 / elapsed)
                  << "";
            std::cout<<"\n";
            std::cout<<"bestmove "<<chess::uci::moveToUci(ret.move)<<"\n"<< std::flush;
            search_state=IDLE;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (input_thread.joinable()) input_thread.join();
}

#endif

