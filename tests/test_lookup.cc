#include <gtest/gtest.h>

#include <iostream>

#include "include/adler32.h"
#include "include/mdict.h"

#define LENTH 255

int getpwd() {
  char pwd[LENTH];
  if (!getcwd(pwd, LENTH)) {
    perror("getcwd");
    return 1;
  }
  printf("getcwd: %s\n", pwd);
  return 0;
}

std::string get_expect_start(const std::string &word) {
  auto *mydict = new mdict::Mdict("../testdict/testdict.mdx");
  mydict->init();
  return mydict->lookup(word);
}

std::string test_lookup(const std::string &word) {
  auto *mydict = new mdict::Mdict("../testdict/testdict.mdx");
  mydict->init();
  return mydict->lookup(word);
}

TEST(mdict, lookup_cake) {
  std::string result = test_lookup("cake");
  EXPECT_STREQ(
      "<font size=+1 ><b>cake</b></font> <br><br><font color=red "
      "><b>noun</b></font><br><b>1</b> <span style=\"COLOR: blue;\"><i>a plate "
      "of cream cakes</i></span> <syn><br><font COLOR= darkblue><b>BUN "
      "</b></font>, pastry, gateau.</syn> <br><xr>See table.</xr> "
      "<br><br><b>2</b> <span style=\"COLOR: blue;\"><i>a cake of "
      "soap</i></span> <syn><br><font COLOR= darkblue><b>BAR </b></font>, "
      "tablet; block, slab, lump, cube, loaf, chunk, brick; piece.</syn> "
      "<br><br><font color=red ><b>verb</b></font><br><b>1</b> <span "
      "style=\"COLOR: blue;\"><i>a pair of boots caked with mud</i></span> "
      "<syn><br><font COLOR= darkblue><b>COVER </b></font>, coat, encrust, "
      "plaster, spread thickly, smother.</syn> <br><br><b>2</b> <span "
      "style=\"COLOR: blue;\"><i>the blood under his nose was beginning to "
      "cake</i></span> <syn><br><font COLOR= darkblue><b>CLOT </b></font>, "
      "congeal, coagulate, thicken; solidify, harden, set, dry; <rl><font "
      "color=\"brown\">rare</font></rl> inspissate.</syn> <br><font "
      "style=\"color: green; \"><b><i>-opposite(s): </i></b></font><font "
      "COLOR=darkblue><b><AX>LIQUEFY</AX>.</b></font><p> <br><br><table "
      "cellpadding=1 cellspacing=0 width=auto style=\"border: 1px solid "
      "black;\"> <tr><td bgcolor=\"EFEFDF\"width=\"3\" style=\"border-bottom: "
      "1px solid black;\">&nbsp;</td><td  style=\"background-color: "
      "EFEFDF;border-bottom: 1px solid black; padding-bottom: 3px;\" "
      "colspan=10><b>CAKES, PUDDINGS, AND DESSERTS</b></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">almond "
      "cake</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">angel (food) cake <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Apfelstrudel <rl><font "
      "color=\"brown\">(German)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">apple "
      "charlotte</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">apple pie</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">baba</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">baked Alaska</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Bakewell tart</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">baklava <rl><font "
      "color=\"brown\">(Middle Eastern)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">banana "
      "split</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">bannock</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">banoffi pie</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">bara brith <rl><font "
      "color=\"brown\">(Welsh)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Bath "
      "bun <rl><font color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Battenberg cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">bavarois</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Berliner</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">black bun <rl><font "
      "color=\"brown\">(Scottish)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Black "
      "Forest gateau</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">blancmange</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">bombe</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">brack <rl><font "
      "color=\"brown\">(Irish)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">bread "
      "pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">bread-and-butter pudding</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Brown "
      "Betty <rl><font color=\"brown\">(N. Amer.)</font></rl></td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">brownie</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">butterfly cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cabinet pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">cassata <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">charlotte</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">charlotte russe</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cheesecake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Christmas cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Christmas pudding <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cobbler <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">coffee cake</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">college pudding <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">compote</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">cream puff</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cr&egrave;me br&ucirc;l&eacute;e</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cr&egrave;me caramel</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cr&ecirc;pes Suzette</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">croquembouche</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">cruller <rl><font "
      "color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">crumble <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">cupcake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Danish pastry</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">death by chocolate</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">devil's food cake <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Dobos torte</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">donut <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">doughnut <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">drop "
      "scone</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">dumpling</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Dundee cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Eccles "
      "cake<rl><font color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">eclair</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">egg custard</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Eve's pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">fairy cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">fancy</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">firni <rl><font "
      "color=\"brown\">(Indian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">flapjack <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">floating island</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">flummery</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">fool</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">frangipane</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">fruit cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">fruit cocktail</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">fruit cup <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">fruit flan</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">frumenty <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">funnel "
      "cake <rl><font color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">gateau</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Genoa cake</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">gingerbread</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">granita <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">halwa "
      "<rl><font color=\"brown\">(Indian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">hasty "
      "pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">hoecake <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">hot cross bun</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">ice cream</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">jam tart</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">jello <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">jelly <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">johnny "
      "cake <rl><font color=\"brown\">(Austral./NZ)</font></rl></td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">junket</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">khir <rl><font "
      "color=\"brown\">(Indian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">kiss "
      "<rl><font color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">kissel</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Knickerbocker Glory "
      "<rl><font color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">koeksister <rl><font color=\"brown\">(S. "
      "African)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">lady's finger <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">lamington <rl><font "
      "color=\"brown\">(Austral./NZ)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">lardy "
      "cake <rl><font color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">layer "
      "cake</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">mac&eacute;doine</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Madeira cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">madeleine</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">marble cake</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">marquise</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">meringue</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">milk pudding <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">millefeuille</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Mississippi mud "
      "pie</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">moon cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">mousse</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">mousseline</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">palacsinta <rl><font "
      "color=\"brown\">(Hungarian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">pancake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">pandowdy <rl><font "
      "color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">panforte <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">parfait</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">parkin <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">pashka "
      "<rl><font color=\"brown\">(Russian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">pavlova</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">peach Melba</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">petit fours</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">plum cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">plum "
      "duff</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">plum pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">popover <rl><font "
      "color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">pound "
      "cake</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">profiteroles</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">queen cake</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">queen of puddings</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">ras "
      "malai <rl><font color=\"brown\">(Indian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">rice "
      "pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">rock cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">roly-poly <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">Sachertorte <rl><font "
      "color=\"brown\">(German)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Sally "
      "Lunn <rl><font color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">sandwich <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">savarin</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">scone</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">seed "
      "cake</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">semolina</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">shoofly pie <rl><font "
      "color=\"brown\">(N. Amer.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">shortcake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">simnel cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">singing hinny <rl><font color=\"brown\">(N. "
      "English)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">snowball</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">sorbet</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">souffl&eacute;</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">sponge cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">sponge pudding <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">spotted dick <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">spumoni <rl><font color=\"brown\">(N. "
      "Amer.)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">steamed pudding</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">streusel <rl><font "
      "color=\"brown\">(German)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">strudel <rl><font "
      "color=\"brown\">(German)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">suet "
      "pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">suji <rl><font "
      "color=\"brown\">(Indian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">summer "
      "pudding</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">sundae</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Swiss roll <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">syllabub</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">tapioca</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">tartufo <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">tea "
      "bread</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">tea cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">tipsy cake <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">tiramisu <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">torte "
      "<rl><font color=\"brown\">(German)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">treacle tart</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">trifle</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">turnover</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">tutti-frutti</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">twinkie <rl><font color=\"brown\">(N. Amer. "
      "trademark)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">upside-down "
      "cake</td></tr> <tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">vetkoek <rl><font color=\"brown\">(S. "
      "African)</font></rl></td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">Victoria sponge <rl><font "
      "color=\"brown\">(Brit.)</font></rl></td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">waffle</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">water ice</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">wedding cake</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">whip</td></tr> <tr><td "
      "bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">yogurt</td></tr> <tr><td bgcolor=\"EFEFDF\" "
      "width=\"3\">&nbsp;</td><td bgcolor=\"EFEFDF\">yule log</td></tr> "
      "<tr><td bgcolor=\"EFEFDF\" width=\"3\">&nbsp;</td><td "
      "bgcolor=\"EFEFDF\">zabaglione <rl><font "
      "color=\"brown\">(Italian)</font></rl></td></tr></table>\r\n",
      result.c_str());
}

TEST(mdict, lookup_case) {
  std::string result = test_lookup("aback");

  EXPECT_STREQ(
      "<font size=+1 ><b>aback</b></font> <br><br><font color=red "
      "><b>adverb</b></font><br> <br><font style=\"COLOR: darkblue;\"><b>take "
      "someone aback</b></font> <span style=\"COLOR: blue;\"><i>Joanna was "
      "taken aback by the violence of his reaction</i></span> <syn><br><font "
      "COLOR= darkblue><b>SURPRISE </b></font>, shock, stun, stagger, astound, "
      "astonish, startle, take by surprise; dumbfound, daze, nonplus, stop "
      "someone in their tracks, stupefy, take someone's breath away; shake "
      "(up), jolt, throw, unnerve, disconcert, disturb, disquiet, unsettle, "
      "discompose, bewilder; <rl><font color=\"brown\">informal</font></rl> "
      "flabbergast, knock for six, knock sideways, knock out, floor, strike "
      "dumb.</syn>\r\n",
      result.c_str());
}

TEST(mdict, lookup_Satan) {
    std::string result = test_lookup("Satan");
  EXPECT_STREQ(
      "<font size=+1 ><b>Satan</b></font> <br><br><font color=red "
      "><b>noun.</b></font><br> <br><xr>See table at <a "
      "href=\"entry://devil\">devil</a>.</xr>\r\n"

      ,
      result.c_str());
}

TEST(mdict, lookup_zoom) {
  std::string result = test_lookup("zoom");
  EXPECT_STREQ(
      "<font size=+1 ><b>zoom</b></font> <br><br><font color=red "
      "><b>verb</b></font><br> <span style=\"COLOR: blue;\"><i>a motorbike "
      "zoomed across their path</i></span> <syn><br><font COLOR= "
      "darkblue><b>SPEED </b></font>, streak, dash, rush, pelt, race, tear, "
      "shoot, blast, flash, fly, wing, scurry, scud, hurry, hasten, scramble, "
      "charge, chase, career, go like lightning, go hell for leather; "
      "<rl><font color=\"brown\">informal</font></rl> whizz, whoosh, vroom, "
      "buzz, hare, zip, whip, belt, scoot, scorch, burn rubber, go like a bat "
      "out of hell; <rl><font color=\"brown\">Brit. informal</font></rl> bomb, "
      "bucket, shift, put one's foot down, go like the clappers; <rl><font "
      "color=\"brown\">Scottish informal</font></rl> wheech; <rl><font "
      "color=\"brown\">N. Amer. informal</font></rl> boogie, hightail, clip, "
      "barrel, lay rubber, get the lead out; <rl><font color=\"brown\">N. "
      "Amer. vulgar slang</font></rl> drag/tear/haul ass; <rl><font "
      "color=\"brown\">informal, dated</font></rl> cut along; <rl><font "
      "color=\"brown\">archaic</font></rl> post, hie, fleet.</syn>\r\n"
      ,
      result.c_str());
}

TEST(mdict, lookup_wisdom) {
  std::string result = test_lookup("wisdom");
  EXPECT_STREQ(
      "<font size=+1 ><b>wisdom</b></font> <br><br><font color=red "
      "><b>noun</b></font><br><b>1</b> <span style=\"COLOR: blue;\"><i>a "
      "number of senior politicians questioned the wisdom of the "
      "decision</i></span> <syn><br><font COLOR= darkblue><b>SAGACITY "
      "</b></font>, sageness, intelligence, understanding, insight, "
      "perception, perceptiveness, percipience, penetration, perspicuity, "
      "acuity, discernment, sense, good sense, common sense, shrewdness, "
      "astuteness, acumen, smartness, judiciousness, judgement, foresight, "
      "clear-sightedness, prudence, circumspection; logic, rationale, "
      "rationality, soundness, saneness, advisability; <rl><font "
      "color=\"brown\">informal</font></rl> sharpness; <rl><font "
      "color=\"brown\">N. Amer. informal</font></rl> savvy, smarts; <rl><font "
      "color=\"brown\">rare</font></rl> sapience, arguteness.</syn> <br><font "
      "style=\"color: green; \"><b><i>-opposite(s): </i></b></font><font "
      "COLOR=darkblue><b><AX>STUPIDITY</AX>, <AX>FOLLY</AX>.</b></font> "
      "<br><br><b>2</b> <span style=\"COLOR: blue;\"><i>the wisdom of the "
      "East</i></span> <syn><br><font COLOR= darkblue><b>KNOWLEDGE "
      "</b></font>, learning, erudition, scholarship, philosophy; "
      "lore.</syn>\r\n",
      result.c_str());
}

// ab initio not found!
// ab ovo not found!
// Africa not found!
// America not found!

TEST(mdict, lookup_ab_initio) {
  std::string result = test_lookup("ab initio");
  EXPECT_STREQ(
      "<font size=+1 ><b>ab initio</b></font> <br><br><font color=red "
      "><b>adverb &amp; adjective</b></font><br> <span style=\"COLOR: "
      "blue;\"><i>the transactions were void ab initio</i></span> "
      "<syn><br><font COLOR= darkblue><b>FROM THE BEGINNING </b></font>, from "
      "the start, initially, originally, from first principles, to begin with, "
      "to start with, primarily; <rl><font color=\"brown\">music</font></rl> "
      "da capo; <rl><font color=\"brown\">informal</font></rl> from scratch, "
      "from the word go.</syn>\r\n",
      result.c_str());
}

TEST(mdict, lookup_Africa) {
  std::string result = test_lookup("Africa");
  EXPECT_STREQ(
      "<font size=+1 ><b>Africa</b></font> <br><br><font color=red "
      "><b>noun.</b></font><br> <br><xr>See table.</xr>\r\n",
      result.c_str());
}

TEST(mdict, lookup_tableau) {
  std::string result = test_lookup("tableau");
  EXPECT_STREQ(
      "<font size=+1 ><b>tableau</b></font> <br><br><font color=red "
      "><b>noun</b></font><br><b>1</b> <span style=\"COLOR: blue;\"><i>the sun "
      "and moon frequently appear in mythic tableaux and they are clearly "
      "there in some symbolic role</i></span> <syn><br><font COLOR= "
      "darkblue><b>PICTURE </b></font>, painting, representation, portrayal, "
      "illustration, image.</syn> <br><br><b>2</b> <span style=\"COLOR: "
      "blue;\"><i>in the first act the action is represented in a series of "
      "tableaux</i></span> <syn><br><font COLOR= darkblue><b>PAGEANT "
      "</b></font>, tableau vivant, human representation, parade, diorama, "
      "scene.</syn> <br><br><b>3</b> <span style=\"COLOR: blue;\"><i>our "
      "entrance disturbed the domestic tableau around the fireplace</i></span> "
      "<syn><br><font COLOR= darkblue><b>SCENE </b></font>, arrangement, "
      "grouping, group; picture, spectacle, image, view, vignette.</syn>\r\n",
      result.c_str());
}

TEST(mdict, lookup_Table_BIOCHEMICAL) {
  std::string result = test_lookup("Table_BIOCHEMICAL");
  EXPECT_STREQ("", result.c_str());
}

/*
tableau not found!
Table_BIOCHEMICAL SUGARS not found!
Table_BRANCHES OF MATHEMATICS not found!
Table_BRANCHES OF PHILOSOPHY not found!
Table_CARDINAL VIRTUES & THEOLOGICAL VIRTUES not found!
Table_CHILDREN'S GAMES not found!
Table_COCKTAILS AND MIXED DRINKS not found!
Table_CONTINENTS OF THE WORLD not found!
Table_CREATURES FROM MYTHOLOGY AND FOLKLORE not found!
Table_CUTS OR JOINTS OF MEAT not found!
Table_DANCES AND TYPES OF DANCING not found!
Table_DIETARY HABITS not found!
Table_DWELLINGS not found!
Table_FAMOUS GANGSTERS not found!
Table_FILM DIRECTORS not found!
Table_FIREWORKS not found!
Table_FLOWERING PLANTS AND SHRUBS not found!
Table_INVENTORS not found!
Table_JAZZ MUSICIANS AND SINGERS not found!
Table_MARSUPIALS not found!
Table_MEASUREMENT UNITS not found!
Table_NAMES OF CANONICAL HOURS not found!
Table_NOBEL PRIZEWINNERS FOR PEACE & NOBEL PRIZEWINNERS FOR ECONOMIC SCIENCES
not found! Table_PHONETIC ALPHABET not found! Table_POISONOUS PLANTS AND FUNGI
not found! Table_PSYCHIATRISTS, PSYCHOLOGISTS, AND PSYCHOANALYSTS not found!
Table_PSYCHOLOGICAL ILLNESSES AND CONDITIONS not found!
Table_rely on; depend; trust not found!
Table_SALAD DRESSINGS not found!
Table_SEASHELLS not found!
tablet not found!
 */

int main(int argc, char **argv) {
  getpwd();

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
