#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include "visutils.h"

#ifdef ITCH
constexpr bool large_font = false;
#else
constexpr bool large_font = true;
#endif

using namespace std;
bool is_mobile;

struct purity_data {
  string cap;
  string shorttext;
  string longtext;
  };

vector<purity_data> purity;

string showing_what;

struct game {
  string shorttitle;
  string pre;
  string title;
  string year;
  vector<pair<string, string>> quotes;
  vector<pair<string, string>> official;
  vector<pair<string, string>> pngs;
  vector<string> funfacts;
  vector<string> contro;
  vector<string> extras;
  map<string, string> meta;
  vector<pair<string, string>> play;
  char verdict;
  map<char, vector<string>> violate;
  };

vector<game> games;

int changes;

string junk;
string* addto = &junk;

bool auto_break = false;

map<char, string> color_codes = {
  {'4', "🟢"}, // green
  {'3', "🟡"}, // yellow
  {'2', "🟠"}, // orange
  {'1', "🟤"}, // brown
  {'0', "🔴"}, // red
  {'x', "🟣"}, // purple
  {'y', "🔵"}, // blue
  {'-', "⚪"}, // white
  {0, "❓"},
  };

map<char, string> rgb_color_codes = {
  {'4', "00FF10"}, // green
  {'3', "FFFF00"}, // yellow
  {'2', "FF8000"}, // orange
  {'1', "804000"}, // brown
  {'0', "FF0000"}, // red
  {'x', "FF00FF"}, // purple
  {'y', "0000FF"}, // blue
  {'-', "FFFFFF"}, // white
  {0, "000000"},
  };

vector<pair<string, string>> metas =
  {
   {"INSPIRED", "INSPIRATIONS"},
   {"INTER", "CONTROL"},
   {"VISUAL", "VISUALS"},
   {"DEATH", "DEATH"},
   {"META", "META"},
   {"RPG", "RPG ELEMENTS"},
   {"RANDOM", "RANDOM ELEMENTS"},
   {"THEME", "THEME"},
   {"STRUCTURE", "WORLD STRUCTURE"},
   {"LENGTH", "RUN LENGTH"},
   {"MODEL", "DEV MODEL"},
   {"COMPLEX", "COMPLEXITY"}
   };

vector<string> default_options = {
  "0:@ is definitely not a roguelike",
  "1:@ is a bit similar to roguelikes, but not much",
  "2:@ has roguelike elements",
  "3:@ is roguelike-like",
  "4:@ is a roguelike",
  "-:@ might be a roguelike but not enough information"
  };

vector<string> split_by_space(string s) {
  s += " ";
  vector<string> res;
  string next;
  for(char c: s) if(c == ' ') { res.push_back(next); next = ""; } else { next += c; }
  return res;
  }

void parse() {
  ifstream f("isitrl.txt");
  string s;
  bool last_empty;

  set<string> metaset;
  for(auto& p: metas) metaset.insert(p.first);

  while(getline(f, s)) {
    if(s == "") {last_empty = true; continue; }

    string cap, param;
    int pos = s.find(" ");
    if(pos != string::npos)  { cap = s.substr(0, pos); param = s.substr(pos+1); }
    else { cap = s; param = ""; }

    auto& g = games.back();

    if(cap == "PURITY") {
      purity.emplace_back();
      auto& p = purity.back();
      pos = param.find(" ");
      if(pos == string::npos) { printf("bad purity format %s\n", param.c_str()); continue; }
      p.cap = param.substr(0, pos);
      p.shorttext = param.substr(pos+1);
      p.longtext = ""; last_empty = false;
      addto = &p.longtext; continue;
      }

    else if(cap == "VIOLATE") {
      g.violate['4'] = split_by_space(param);
      }

    else if(cap == "VIOLATEX") {
      g.violate[param[0]] = split_by_space(param.substr(2));
      }

    else if(cap == "GAME") {
      games.emplace_back();
      auto& g1 = games.back();
      if(param[2] != ':') { printf("bad game format %s\n", param.c_str()); continue; }
      g1.shorttitle = param.substr(0, 2);
      g1.title = param.substr(3);
      g1.verdict = 0;
      addto = &junk;
      for(auto opt: default_options) {
        int pos = opt.find("@");
        opt.replace(pos, 1, g1.title);
        g1.extras.push_back(opt);
        }
      }

    else if(cap == "YEAR") {
      g.year = param;
      addto = &junk;
      }

    else if(cap == "OFFICIAL") {
      g.official.emplace_back();
      g.official.back().first = param;
      addto = &g.official.back().second;
      last_empty = false; continue;
      }

    else if(cap == "PRE") {
      g.pre = "";
      addto = &g.pre;
      last_empty = false; continue;
      }

    else if(cap == "QUOTE") {
      g.quotes.emplace_back();
      g.quotes.back().first = param;
      addto = &g.quotes.back().second;
      last_empty = false; continue;
      }

    else if(cap == "#") {}

    else if(cap == "PNG") {
      pos = param.find(" ");
      if(pos == string::npos) { printf("bad png format %s\n", param.c_str()); continue; }
      g.pngs.emplace_back(param.substr(0, pos), param.substr(pos+1));
      addto = &junk;
      }

    else if(cap == "FUNFACT") {
      g.funfacts.emplace_back();
      addto = &g.funfacts.back();
      last_empty = false; continue;
      }

    else if(cap == "CONTRO") {
      g.contro.emplace_back();
      addto = &g.contro.back();
      last_empty = false; continue;
      }

    else if(cap == "EXTRA") {
      g.extras.emplace_back(param);
      addto = &junk;
      }
    
    else if(cap == "PLAY") {
      pos = param.find(" ");
      if(pos == string::npos) { printf("bad png format %s\n", param.c_str()); continue; }
      g.play.emplace_back(param.substr(0, pos), param.substr(pos+1));
      addto = &junk;
      }
    
    else if(metaset.count(cap)) {
      if(param == "*") { g.meta[cap] = "<br/>"; addto = &g.meta[cap]; last_empty = false; continue; }
      else { g.meta[cap] = param; }
      }

    else {
      if(last_empty && *addto != "") *addto += "<br/><br/>";
      *addto += s; *addto += " "; auto_break = true;
      }

    last_empty = false;
    }

  printf("%s\n", junk.c_str());
  }

void reset_all() {
  for(auto& g: games) g.verdict = 0;
  }

void add_button(ostream& of, string action, string text) {
  if(is_mobile && !large_font)
     of << "<button style='width:100%;font-size:1.5rem' type='button' onclick='" << action <<
       "'/>" << text << "</button>";
  else if(is_mobile)
     of << "<button style='font-size:3rem;width:100%' type='button' onclick='" << action << 
       "'/>" << text << "</button>";
  else
     of << "<button type='button' onclick='" << action << "'/>" << text << "</button>";
  }

void display(ostream& of, game& g) {
  if(!is_mobile) {
    of << "<div style=\"float:left;width:100%\">\n";
    of << "<div style=\"float:left;width:10%\">&nbsp;</div>";
    of << "<div style=\"float:left;width:40%\">";
    of << "<br/>";
    for(auto &png: g.pngs) {
      of << "<table><tr>";
      of << "<TD ALIGN=CENTER VALIGN=BOTTOM>";
      of << "<A target=\"_blank\" href=\"pngs/" + png.first + ".png\" style=\"text-decoration:none\"><IMG src=\"pngs/" + png.first + ".png\" width=\"90%\" style=\"border: 5x solid %55;\"/>";
      of << "<div style=\"text-align: center\">" + png.second + "</div></A></TD></tr></table>";
      of << "<br/>";
      }
    of << "<br/>";
    if(g.pngs.empty()) of << "(no picture)";
    of << "</div>\n";
    of << "<div style=\"float:left;width:40%\">";
    }
  of << "<h1>" << g.year << "</h1>\n";
  if(g.pre != "") {
    of << g.pre << "<br/><br/>";
    }
  for(auto &ofc: g.official) {
    of << "<b><font color=\"8080FF\">Official description: " << ofc.first << "</font></b><br/>";
    of << ofc.second << "<br/><br/>";
    }
  if(is_mobile) {
    for(auto &png: g.pngs) {
      of << "<table><tbody><tr>";
      of << "<TD ALIGN=CENTER VALIGN=BOTTOM>";
      of << "<A target=\"_blank\" href=\"pngs/" + png.first + ".png\" style=\"text-decoration:none\"><IMG src=\"pngs/" + png.first + ".png\" width=\"90%\" BORDER=\"2\" class=\"noborder\"/>";
      of << "<div style=\"text-align: center\">" + png.second + "</div></A></TD></tr></tbody></table>";
      of << "<br/>";
      }
    }
  for(auto &ofc: g.quotes) {
    of << "<b><font color=\"8080FF\">Quoting " << ofc.first << "</font></b><br/>";
    of << ofc.second << "<br/><br/>";
    }
  for(auto& p: g.play)
    of << "<b><font color=\"FF80FF\">Play: </font><a target=\"_blank\" href=\"" + p.first + "\">" + p.second + "</a></b><br/><br/>";
  for(auto &ff: g.funfacts) {
    of << "<b><font color=\"80FF80\">Fun fact:</font></b><br/>" << ff << "<br/><br/>";
    }
  for(auto &cf: g.contro) {
    of << "<b><font color=\"FF8080\">Controversies?</font></b><br/>" << cf << "<br/><br/>";
    }

  for(auto& me: metas) if(g.meta.count(me.first))
    of << "<b><font color=\"FFFF80\">" << me.second << "</font></b> " << g.meta[me.first] << "<br/>";

  of << "<br/>";
  auto radio = [&] (string val, string s) {
    add_button(of, "answer(\"" + g.shorttitle + "\", \"" + val + "\")", s);
    of << "<br/>";
    };
  for(auto extra: g.extras) {
    radio(s0 + (extra[0]), extra.substr(2));
    }
  of << "<br/>";
  if(!is_mobile) {
    of << "</div>\n";
    of << "</div>\n";
    }
  }

void add_buttons(ostream& ss) {
  ss << "<center>";
  ss << "<a onclick=\"back()\">go back</a> - ";
  bool more = false;
  for(auto& g: games) if(!g.verdict && g.shorttitle != "II") more = true;
  if(more) ss << "<a onclick=\"next()\">next</a> - ";
  else ss << "<a onclick=\"results()\">results</a> - ";
  ss << "<a onclick=\"help()\">help</a>";
  ss << " - <a onclick=\"s_import('')\">import</a>";
  ss << " - <a onclick=\"compare()\">compare</a>";
  if(changes) {
    ss << " - <a onclick=\"suggestions()\">suggestions</a>";
    }
  ss << "</center>";
  }

void show_results();

bool finished() {
  for(auto& g: games) if(!g.verdict && g.shorttitle != "II") return false;
  return true;
  }

void show_next() {
  for(auto& g: games) if(!g.verdict && g.shorttitle != "II") {
    showing_what = g.shorttitle;
    stringstream ss;
    display(ss, g);

    ss << "<hr/>\n";
    add_buttons(ss);
    set_value("output", ss.str());
    return;
    }

  show_results();  
  }

void single_page_view() {
  stringstream ss;
  showing_what = "singlepage";

  for(auto &g: games) if(g.shorttitle != "II") {
    // display(of, g);
    // of << "<hr/>\n";
    display(ss, g);
    ss << "<hr/>\n";
    }

  add_buttons(ss);
  // of << "</body></html>\n";
  set_value("output", ss.str());
  }

string code, mainpart;

string rogue_code(string sep) {
  string s = "";
  for(auto& g: games) if(g.verdict) {
    s+= g.shorttitle + g.verdict;
    s += sep;
    }
  return s;
  }

void parse_rogue_code(string s) {
  for(int i=0; i<int(s.size()-2); i++) if(isalnum(s[i])) {
    string t = "";
    t += s[i++];
    t += s[i++];
    char ver = s[i];
    for(auto& g: games) if(g.shorttitle == t) g.verdict = ver;
    }
  }

void rogue_code_out(ostream& ss, string linkpart) {
  ss << "Your Roguelike Code is: <br/><tt><font size=\"200%\">" << rogue_code(" ") << "</font></tt><br/><br/>";
  string link = mainpart + "?" + linkpart + "=" + rogue_code("");
  ss << "Link: <a href=\"" + link + "\">link</a><br/><br/>";
  }

void show_details() {
  stringstream ss;
  showing_what = "details";
  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  for(auto& p: purity) {
    ss << "<b>" << p.shorttext << "</b><br/>";
    if(p.longtext != "")
    ss << p.longtext << "<br/>";
    vector<string> violators, abiders;
    for(auto& g: games) {
      bool abide = true;
      // ss << g.title << ":";
      // for(auto& x: g.violate['4']) ss << " " << x; ss << " VS " << p.cap << "<br/>";
      for(auto& x: g.violate['4']) if(x == p.cap) abide = false;
      if(abide) abiders.push_back(color_codes[g.verdict] + " " + g.title);
      else violators.push_back(color_codes[g.verdict] + " " + g.title);
      }
    ss << "<br/>Violators:<br/><ul>";
    for(auto& v: violators) ss << "<li>" << v << "</li>";
    ss << "</ul><br/>Abiders:<br/><ul>";
    for(auto& v: abiders) ss << "<li>" << v << "</li>";
    ss << "</ul><br/>";
    }

  if(!is_mobile) ss << "</div></div>";
  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_init() {
  stringstream ss;
  showing_what = "init";
  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << 
  "This is a quiz where you decide whether a game is a roguelike. On the way, you will hopefully "
  "learn about the history of this genre, and some great games. Have fun!<br/><br/>"

  "There are no 'correct' answers (even if we, and people we cite, try to convince you what the correct answer is), but in the end, your answers will be analyzed.<br/><br/>"

  "You probably just want to jump straight in. "
  "Most things will be explained as you go, and as new games challenge earlier interpretations.<br/><br/>"
  "But if you want more explanations, you can always click the \"help\" button "
  "below. Back button may be used to edit your earlier answers, or save your progress to a link.<br/><br>";

  add_button(ss, "next()", "Start!");

  ss << "<br/><br/>";

  if(!is_mobile) {
    ss << "</div></div>";
    }
  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_results() {
  stringstream ss;
  showing_what = "results";
  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << "Your answers:<br/>";

  ss << "<ul>";
  for(auto& g: games) if(g.verdict) {
    for(auto x: g.extras) if(x[0] == g.verdict)
      ss << "<li>" << color_codes[g.verdict] << " " << x.substr(2) << "</a></li>";
    }

  ss << "</ul>";

  ss << "<br/><b>PURITY</b><br/><br/>";

  map<string, pair<int, int>> violations;

  map<string, vector<string>> who_violated;

  for(auto& g: games) if(g.verdict) {
    for(auto x: g.violate['4']) violations[x].second++;
    for(auto x: g.violate[g.verdict]) {
      violations[x].first++;
      who_violated[x].push_back(g.title);
      }
    }

  int qpure = 0, qviolate = 0;
  for(auto& v: violations) if(v.second.first == 0 && v.second.second) qpure++; else qviolate++;

  int qty_roguelikes = 0;
  int qty_nonroguelikes = 0;
  int qty_matching = 0;
  vector<string> surprises;

  for(auto& g: games) if(g.verdict) {
    bool ok = true;
    for(auto& v: g.violate['4']) if(violations[v].first == 0) {
      ok = false;
      }

    if(g.verdict == '4' || (g.title != "Rogue" && (g.verdict == 'x' || g.verdict == 'y'))) {
      qty_roguelikes++; qty_matching++;
      }
    else {
      qty_nonroguelikes++;
      if(ok) { qty_matching++; surprises.push_back(g.title); }
      }
    }

  if(qty_nonroguelikes == 0)
    ss << "Everything is a roguelike! But then, don't you think that video game genres should mean something?<br/>";
  else if(qty_roguelikes == 0)
    ss << "Nothing is a roguelike! Welcome, <a target=\"_noblank\" href=\"https://hard-drive.net/hd/video-games/roguelike-genre-purist-hopes-someone-will-develop-a-roguelike-someday/\">Noreen Ramirez</a>.<br/>";
  else if(qpure == 0) ss << "It appears there is no element that a roguelike must have.<br/>";
  else if(qpure == 1) ss << "Based on your answer, it appears there is one element that a roguelike must have:<br/>";
  else ss << "Based on your answer, it appears that:<br/>";

  if(qpure) {
    ss << "<ul>";
    for(auto& p: purity) {
      auto& v = violations[p.cap];
      if(v.first == 0 && v.second)
      ss << "<li>" << p.shorttext << " (" << v.second << ")<br/>";
      }
    ss << "</ul>";

    if(qpure > 1) {
      ss << "It is possible that you do not actually consider some of these criteria to be essential (but then, many serious roguelike definitions include such red herrings).<br/><br/>";
      }

    if(qty_roguelikes == qty_matching) {
      ss << "You consider all " << qty_roguelikes << " games matching these criteria to be roguelikes.<br/>";
      }
    else {
      ss << "You consider " << qty_roguelikes << " out of " << qty_matching << " games matching these criteria to be roguelikes.<br/>";
      ss << "These appear to not be roguelikes due to some other criteria:";
      bool notfirst = false;
      for(auto &g: surprises) { if(notfirst) ss << ","; notfirst = true; ss << " " << g; }
      ss << "<br/>";
      }
    }

  if(qviolate) {
    ss << "<br/>You apparently do not agree with the following statements:<br/>";
    ss << "<ul>";
    for(auto& p: purity) {
      auto& v = violations[p.cap];
      if(v.first > 0) {
        ss << "<li>" << p.shorttext << " (";
        if(v.first < 3) {
          for(auto &violator: who_violated[p.cap]) ss << violator << " ";
          }
        else ss << v.first;
        ss << "/" << v.second << ")</li>";
        }
      }
    ss << "</ul>";
    }

  ss << "<br/><br/>";
  ss << "Please click <a onclick=\"details()\">here</a> to see what exactly do these criteria mean.<br/><br/>";

  if(changes) {
    string link = mainpart + "?r=" + rogue_code("");

    ss << "Thanks for playing this! Click <a onclick=\"suggestions()\">this</a> to make it better, or ";
   ss << "<a onclick='tweet(\"My #isitrl results!\", \"" + link + "\")'>share the link to results to social media with #isitrl tag</a>. <br/><br/>";
   }
  
  rogue_code_out(ss, "r");
  
  if(!is_mobile) {
    ss << "</div></div>";
    }

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_back_to(string what) {

  for(auto& g: games) if(g.shorttitle == what) {
    stringstream ss;
    display(ss, g);

    ss << "<hr/>\n";
    add_buttons(ss);
    set_value("output", ss.str());
    return;
    }

  stringstream ss;
  showing_what = "not found";
  ss << "not found";
  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_help() {
  show_back_to("II");
  }

void show_back() {

  stringstream ss;
  showing_what = "back";

  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << "<br/>Changing your decisions is cheating, but we won't stop you doing it!<br/><br/>";
  ss << "Click on one of the following buttons to re-read a game description or to change your opinion.<br/><br/>";

  for(auto& g: games) if(g.verdict) {
    for(auto x: g.extras) if(x[0] == g.verdict)
      ss << "<a onclick='back_to(\"" << g.shorttitle << "\")'>" << color_codes[g.verdict] << " " << x.substr(2) << "</a><br/>";
    }

  ss << "<br/><br/>";
  rogue_code_out(ss, "b");
  if(!is_mobile) ss << "</div></div>";

  for(auto& g: games) for(auto& g2: games) if(g.shorttitle == g2.shorttitle && &g != &g2) {
    ss << "WARNING: double code " << g.shorttitle << "<br/>";
    }

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_import(const string& code) {

  parse_rogue_code(code);

  stringstream ss;
  showing_what = "";

  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << "Just like you could in <i>Beneath Apple Manor</i>, you can write down your Roguelike Code on a piece on paper, ";
  ss << "and then, enter it here to restore your progress. You could also just use the link next to Roguelike Code, but that would be boring.<br/><br/>";

  ss << "<input id=\"importcode\" length=40 type=text/><br/><br/>";

  add_button(ss, "s_import(document.getElementById(\"importcode\").value)", "import");

  ss << "<br/><br/>";

  rogue_code_out(ss, "i");

  ss << "You can also press ";
  add_button(ss, "resetquiz()", "reset!");
  ss << " to reset your progress.<br/><br/>";

  if(!is_mobile) ss << "</div></div>";

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_compare() {
  stringstream ss;
  showing_what = "";

  ss << "Enter the answers to compare here:<br/><br/>";

  ss << "<textarea id=\"aggregatedata\" rows=50 cols=180>";

  ifstream f("answers.txt");
  string s;
  while(getline(f, s)) ss << s << "\n";

  ss << "</textarea><br/><br/>";

  add_button(ss, "aggregate(document.getElementById(\"aggregatedata\").value)", "compare");

  ss << "<br/><br/>";

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_aggregate(const string& s) {
  showing_what = "";
  string cur = "";
  string who;
  map<string, string> codes;
  for(char ch: s + "\n") {
    if(ch == '\n') {
      if(cur.back() == ':') { who = cur; who.pop_back(); }
      else if(who != "") { codes[who] = cur; who = ""; }
      cur = "";
      }
    else cur += ch;
    }
  string mycode = rogue_code("");
  codes["YOU"] = mycode;
  map<string, string> verdicts;
  for(auto& p: codes) {
    reset_all();
    parse_rogue_code(p.second);
    string sh;
    for(auto& g: games) sh += g.verdict;
    verdicts[p.first] = sh;
    }

  reset_all();
  parse_rogue_code(mycode);

  stringstream ss;

  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  string you = verdicts["YOU"];
  verdicts.erase("YOU");

  struct comparison {
    int score, outof;
    string who;
    map<char, string> differences;
    };

  vector<comparison> comparisons;

  for(auto& v: verdicts) {
    string& they = v.second;
    comparisons.emplace_back();
    auto& c = comparisons.back();
    c.who = v.first;

    c.score = 0, c.outof = 0;
    map<char, string> differences;

    for(int i=0; i<int(games.size()); i++) {
      if(you[i] == 0 || they[i] == 0) continue;
      if(you[i] != they[i]) {
        c.differences[you[i]] += " " + color_codes[they[i]] + " " + games[i].title;
        }
      if(you[i] == '-' || they[i] == '-') continue;
      if(you[i] == they[i]) { c.score += 2; c.outof += 2; continue; }
      if('0' <= you[i] && you[i] <= '4' && '0' <= they[i] && '4' <= they[i] && abs(you[i] - they[i]) >= 2) { c.score += 0; c.outof += 2; continue; }
      c.score += 1; c.outof += 2; continue;
      }

    if(c.outof == 0) { comparisons.pop_back(); continue; }
    }
  sort(comparisons.begin(), comparisons.end(), [] (comparison& a, comparison& b) { return a.score * b.outof > b.score * a.outof; });
  if(comparisons.size() > 0) {
    ss << "Your answers versus other answers:<ul>\n";
    for(auto& c: comparisons) {
      char buf[64]; sprintf(buf, "%5.1f%%", 100. * c.score / c.outof);
      ss << "<li><b>" << c.who << "</b> (similarity: " << buf << ")<ul>\n";
      for(auto& d: c.differences) {
        ss << "<li>you: " << color_codes[d.first] << " they:" << d.second << "</li>\n";
        }
      ss << "</ul></li>\n";
      }
    ss << "</ul>\n";
    ss << "How is similarity computed:<br/>No answers and " << color_codes['-'] << " are ignored; same answers = full similarity, at least two steps in "
     << color_codes['0'] + color_codes['1'] + color_codes['2'] + color_codes['3'] + color_codes['4'] << " = no similarity, otherwise half similarity<br/><br/>";
    }

  verdicts["YOU"] = you;

  ss << "<b>Aggregate results:</b><br/>";

  if(comparisons.size() > 0) ss << "(Note: your opinions are included here)<br/>";

  ss << "<table cellpadding=0 cellspacing=0>\n";
  for(int index = 0; index < int(games.size()); index++) {
    map<char, int> qty;
    int total = 0;
    for(auto& v: verdicts) if(v.second[index]) {
      total++; qty[v.second[index]]++;
      }
    if(!total) continue;
    ss << "<tr><td style=\"celling-top:0px;\">" << games[index].title << "</td>";
    int sofar = 0;
    double width = 200, height = 16;
    ss << "<td style=\"celling-top:0px;\"><svg style=\"border: 0px;\" width=\"" << width << "\" height=\"" << height << "\">";
    for(auto p: qty) {
      double left = sofar * width / total;
      sofar += p.second;
      double right = sofar * width / total;
      ss << "<path d=\"M " << left << " 0 L " << left << " " << height << " L " << right << " " << height << " L " << right << " 0 Z\" ";
      ss << "style=\"stroke:#000000;stroke-opacity:1;stroke-width:1px;fill:#" << rgb_color_codes[p.first] << ";fill-opacity:1\"/>";
      }
    ss << "</svg></td></tr>\n";
    }

  ss << "</table></tr><br/>";

  ss << "Aggregate results -- number details:<br/><ul>";
  for(int index = 0; index < int(games.size()); index++) {
    map<char, int> qty;
    int total = 0;
    for(auto& v: verdicts) if(v.second[index]) {
      total++; qty[v.second[index]]++;
      }
    if(!total) continue;
    ss << "<li>" << games[index].title;
    char buf[64];
    for(auto p: qty) {
      sprintf(buf, "%5.1f", p.second * 100. / total);
      ss << buf << "% " << color_codes[p.first];
      }
    ss << " (out of " << total << ")";
    ss << "</li>";
    }
  ss << "</ul>";

  if(!is_mobile) ss << "</div></div>";

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

void show_suggestions() {

  stringstream ss;
  showing_what = "";

  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << "Just as in actual roguelikes, <b>YOU</b> can make this quiz better!<br/><br/>Corrections, suggestions, ";
  ss << "etc. are welcome.<br/><br/>You can ";
  ss << "post an issue on <a target=\"_blank\" href=\"https://github.com/zenorogue/isitrl\">GitHub</a>, ";
  ss << "comment on <a target=\"_blank\" href=\"https://zenorogue.itch.io/isitrl\">itch.io</a>, ";
  ss << "comment on <a target=\"_blank\" href=\"https://www.reddit.com/r/isitroguelike/comments/1b8w194/is_it_roguelike_quiz/\">Reddit</a>, ";
  ss << "or just send it here.<br/><br/>";
  ss << "Just sharing your answers, nickname, and your experiences with roguelikes may be used to make it better, as we could include statistical data in the future!<br/><br/>";

  ss << "<form method=\"post\" action=\"https://roguetemple.com/z/isitrl.php\" name=\"submit\">\n";
  ss << "<center><textarea name=\"comment\" rows=20 cols=80>\n";
  ss << "My Roguelike Code: " << rogue_code(" ") << "\n\n";
  ss << "write here!";
  ss << "</textarea><br><br>\n";
  ss << "<input type=\"hidden\" name=\"hidden\" value=\"3487\">\n";
  ss << "<input type=\"submit\" name=\"submit\" value=\"send\">\n";
  ss << "</center></form>\n";
  ss << "<br/><br/>";
  if(!is_mobile) ss << "</div></div>";

  ss << "<hr/>\n";
  add_buttons(ss);
  set_value("output", ss.str());
  }

int init(bool _is_mobile) {
  is_mobile = _is_mobile;

  parse();

  string href = (char*)EM_ASM_INT({
    var jsString = document.location.href;
    var lengthBytes = lengthBytesUTF8(jsString)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(jsString, stringOnWasmHeap, lengthBytes+1);
    return stringOnWasmHeap;
    });

  char which = 'z';

  auto pos = href.find("?");
  if(pos == string::npos) {
    code = "";
    mainpart = href;
    }
  else {
    mainpart = href.substr(0, pos);
    which = href[pos+1];
    code = href.substr(pos+3);
    parse_rogue_code(code);
    }

  if(which == 'z') show_init();
  else if(which == 'h') show_help();
  else if(which == 'r') show_results();
  else if(which == 'b') show_back();
  else if(which == 'd') show_details();
  else if(which == 'i') show_import("");
  else show_next();

  /*
ofstream of("isitrl.html");

  of << R"a(
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<meta name="author" content="Zeno Rogue">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style type="text/css"> img { max-width: 100%; height: auto; } </style>
<link href="zeno.css" type=text/css rel=stylesheet>
<meta name="copyright" content="&copy; 2024 Zeno Rogue">
<meta name="description" content="a quiz on roguelike history"/>
<meta name="keywords" content="rogue, roguelike">
<title>Is it roguelike?</title>
</head><body>
)a";
*/

  return 0;
  }

void keydown(const string& s) {
  for(auto& g: games) if(g.shorttitle == showing_what)
  for(auto& ge: g.extras) if(ge[0] == s[0]) {
    changes++;
    g.verdict = s[0];
    show_next();
    return;
    }
  if(showing_what != "") {
    if(s == "b") show_back();
    if(s == "n") show_next();
    if(s == "r" && finished()) show_results();
    if(s == "h") show_help();
    if(s == "i") show_init();
    if(s == "c") show_compare();
    if(s == "d" && finished()) show_details();
    }
  }

extern "C" {
  void start(bool mobile) { init(mobile); }

  void next() { show_next(); }
  void results() { show_results(); }
  void back() { show_back(); }
  void back_to(const char *name) { show_back_to(name); }
  void help() { show_help(); }
  void details() { show_details(); }
  void suggestions() { show_suggestions(); }
  void resetquiz() { reset_all(); show_init(); }
  void on_keydown(const char *s) { keydown(s); }
  void do_import(const char *s) { show_import(s); }
  void compare() { show_compare(); }
  void aggregate(const char *s) { show_aggregate(s); }
  void answer(const char *name, const char *result) {
    for(auto& g: games) if(g.shorttitle == name) g.verdict = result[0];
    changes++;
    show_next();
    }
  }
