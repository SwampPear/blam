#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "tokenizer/tokenizer.hpp" // For consistency with your setup, even if unused
#include "tokenizer/llist.hpp"     // <-- Adjust path to where your LList is defined

using namespace Blam;

struct Dummy
{
  int value;
  Dummy(int v) : value(v) {}
};

std::shared_ptr<LListNode<Dummy>> makeNode(int v)
{
  auto node = std::make_shared<LListNode<Dummy>>();
  node->data = std::make_shared<Dummy>(v);
  return node;
}

TEST_CASE("Replace head node", "[llist]")
{
  LList<Dummy> list;

  auto n1 = makeNode(1);
  auto n2 = makeNode(2);
  auto replacement = makeNode(99);

  n1->next = n2;
  n2->prev = n1;
  list.head = n1;

  list.replace(n1, replacement);

  REQUIRE(list.head == replacement);
  REQUIRE(replacement->next == n2);
  REQUIRE(n2->prev == replacement);
  REQUIRE(n1->next == nullptr);
  REQUIRE(n1->prev == nullptr);
}

TEST_CASE("Replace middle node with single replacement", "[llist]")
{
  LList<Dummy> list;

  auto n1 = makeNode(1);
  auto n2 = makeNode(2);
  auto n3 = makeNode(3);
  auto replacement = makeNode(99);

  n1->next = n2;
  n2->prev = n1;
  n2->next = n3;
  n3->prev = n2;
  list.head = n1;

  list.replace(n2, replacement);

  REQUIRE(n1->next == replacement);
  REQUIRE(replacement->prev == n1);
  REQUIRE(replacement->next == n3);
  REQUIRE(n3->prev == replacement);
  REQUIRE(n2->next == nullptr);
  REQUIRE(n2->prev == nullptr);
}

TEST_CASE("Replace with multi-node replacement list", "[llist]")
{
  LList<Dummy> list;

  auto n1 = makeNode(1);
  auto n2 = makeNode(2);
  auto n3 = makeNode(3);

  auto repl1 = makeNode(99);
  auto repl2 = makeNode(100);
  repl1->next = repl2;
  repl2->prev = repl1;

  n1->next = n2;
  n2->prev = n1;
  n2->next = n3;
  n3->prev = n2;
  list.head = n1;

  list.replace(n2, repl1);

  REQUIRE(n1->next == repl1);
  REQUIRE(repl1->prev == n1);
  REQUIRE(repl2->next == n3);
  REQUIRE(n3->prev == repl2);
  REQUIRE(n2->next == nullptr);
  REQUIRE(n2->prev == nullptr);
}

TEST_CASE("Replace tail node", "[llist]")
{
  LList<Dummy> list;

  auto n1 = makeNode(1);
  auto n2 = makeNode(2);
  auto replacement = makeNode(99);

  n1->next = n2;
  n2->prev = n1;
  list.head = n1;

  list.replace(n2, replacement);

  REQUIRE(n1->next == replacement);
  REQUIRE(replacement->prev == n1);
  REQUIRE(replacement->next == nullptr);
  REQUIRE(n2->next == nullptr);
  REQUIRE(n2->prev == nullptr);
}

TEST_CASE("Replace with null victim or replacement does nothing", "[llist]")
{
  LList<Dummy> list;

  auto n1 = makeNode(1);
  auto n2 = makeNode(2);
  n1->next = n2;
  n2->prev = n1;
  list.head = n1;

  list.replace(nullptr, n2);
  REQUIRE(list.head == n1);

  list.replace(n1, nullptr);
  REQUIRE(list.head == n1);
}
