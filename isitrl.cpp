#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include "visutils.h"

using namespace std;
bool is_mobile;

struct purity_data {
  string cap;
  string shorttext;
  string longtext;
  };

vector<purity_data> purity;

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

void add_button(ostream& of, string action, string text) {
  if(is_mobile)
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
  if(changes) {
    ss << " - <a onclick=\"suggestions()\">suggestions</a>";
    }
  ss << "</center>";
  }

void show_results();

void show_next() {
  for(auto& g: games) if(!g.verdict && g.shorttitle != "II") {
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

  if(qpure == 0) ss << "It appears there is no element that a roguelike must have.<br/>";
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
  ss << "Please click <a onclick=\"details()\">here</a> for details.<br/><br/>";

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

  set_value("output", "not found");
  }

void show_help() {
  show_back_to("II");
  }

void show_back() {

  stringstream ss;

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

void show_suggestions() {

  stringstream ss;

  if(!is_mobile) {
    ss << "<div style=\"float:left;width:100%\">\n";
    ss << "<div style=\"float:left;width:30%\">&nbsp;</div>";
    ss << "<div style=\"float:left;width:40%\">";
    }

  ss << "Just as in actual roguelikes, <b>YOU</b> can make this quiz better!<br/><br/>Corrections, suggestions, ";
  ss << "etc. are welcome.<br/><br/>You can post an issue on GitHub, comment on itch.io, or just send it here.<br/><br/>";
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

extern "C" {
  void start(bool mobile) { init(mobile); }

  void next() { show_next(); }
  void results() { show_results(); }
  void back() { show_back(); }
  void back_to(const char *name) { show_back_to(name); }
  void help() { show_help(); }
  void details() { show_details(); }
  void suggestions() { show_suggestions(); }
  void resetquiz() { for(auto& g: games) g.verdict = 0; show_init(); }   
  void do_import(const char *s) { show_import(s); }
  void answer(const char *name, const char *result) {
    for(auto& g: games) if(g.shorttitle == name) g.verdict = result[0];
    changes++;
    show_next();
    }
  }
