
#include "c-interface/c-interface.h"

//=============================================================================
//    Console Methods.
//=============================================================================

ConsoleMethod(ArrayObject, getIndexFromValue, ConsoleInt, 3, 3,
   "Search the array from the current position for the element "
   "@param value Array value to search for\n"
   "@return Index of the first element found, or -1 if none\n")
{
   return object->getIndexFromValue(argv[2]);
}

ConsoleMethod(ArrayObject, getIndexFromKey, ConsoleInt, 3, 3,
   "Search the array from the current position for the key "
   "@param value Array key to search for\n"
   "@return Index of the first element found, or -1 if none\n")
{
   return object->getIndexFromKey(argv[2]);
}

ConsoleMethod(ArrayObject, getValue, ConsoleString, 3, 3,
   "Get the value of the array element at the submitted index.\n"
   "@param index 0-based index of the array element to get\n"
   "@return The value of the array element at the specified index, "
   "or \"\" if the index is out of range\n")
{
   return object->getValueFromIndex(dAtoi(argv[2]));
}

ConsoleMethod(ArrayObject, getKey, ConsoleString, 3, 3,
   "Get the key of the array element at the submitted index.\n"
   "@param index 0-based index of the array element to get\n"
   "@return The key associated with the array element at the "
   "specified index, or \"\" if the index is out of range\n")
{
   return object->getKeyFromIndex(dAtoi(argv[2]));
}

ConsoleMethod(ArrayObject, setKey, ConsoleVoid, 4, 4,
   "Set the key at the given index.\n"
   "@param key New key value\n"
   "@param index 0-based index of the array element to update\n")
{
   object->setKey(argv[2], dAtoi(argv[3]));
}

ConsoleMethod(ArrayObject, setValue, ConsoleVoid, 4, 4,
   "Set the value at the given index.\n"
   "@param value New array element value\n"
   "@param index 0-based index of the array element to update\n")
{
   object->setValue(argv[2], dAtoi(argv[3]));
}

ConsoleMethod(ArrayObject, count, ConsoleInt, 2, 2,
   "Get the number of elements in the array.")
{
   return (S32)object->count();
}

ConsoleMethod(ArrayObject, countValue, ConsoleInt, 3, 3,
   "Get the number of times a particular value is found in the array.\n"
   "@param value Array element value to count\n")
{
   return (S32)object->countValue(argv[2]);
}

ConsoleMethod(ArrayObject, countKey, ConsoleInt, 3, 3,
   "Get the number of times a particular key is found in the array.\n"
   "@param key Key value to count\n")
{
   return (S32)object->countKey(argv[2]);
}

ConsoleMethod(ArrayObject, add, ConsoleVoid, 4, 4,
   "Adds a new element to the end of an array (same as push_back()).\n"
   "@param key Key for the new element\n"
   "@param value Value for the new element\n")
{
   object->push_back(argv[2], argv[3]);
}

ConsoleMethod(ArrayObject, push_back, ConsoleVoid, 4, 4,
   "Adds a new element to the end of an array.\n"
   "@param key Key for the new element\n"
   "@param value Value for the new element\n")
{
   object->push_back(argv[2], argv[3]);
}

ConsoleMethod(ArrayObject, push_front, ConsoleVoid, 4, 4,
   "Adds a new element to the front of an array")
{
   object->push_front(argv[2], argv[3]);
}

ConsoleMethod(ArrayObject, insert, ConsoleVoid, 5, 5,
   "Adds a new element to a specified position in the array.\n"
   "- @a index = 0 will insert an element at the start of the array (same as push_front())\n"
   "- @a index = %array.count() will insert an element at the end of the array (same as push_back())\n\n"
   "@param key Key for the new element\n"
   "@param value Value for the new element\n"
   "@param index 0-based index at which to insert the new element")
{
   object->insert(argv[2], argv[3], dAtoi(argv[4]));
}

ConsoleMethod(ArrayObject, pop_back, ConsoleVoid, 2, 2,
   "Removes the last element from the array")
{
   object->pop_back();
}

ConsoleMethod(ArrayObject, pop_front, ConsoleVoid, 2, 2,
   "Removes the first element from the array")
{
   object->pop_front();
}

ConsoleMethod(ArrayObject, erase, ConsoleVoid, 3, 3,
   "Removes an element at a specific position from the array.\n"
   "@param index 0-based index of the element to remove\n")
{
   object->erase(dAtoi(argv[2]));
}

ConsoleMethod(ArrayObject, empty, ConsoleVoid, 2, 2,
   "Emptys all elements from an array")
{
   object->empty();
}

ConsoleMethod(ArrayObject, uniqueValue, ConsoleVoid, 2, 2,
   "Removes any elements that have duplicated values (leaving the first instance)")
{
   object->uniqueValue();
}

ConsoleMethod(ArrayObject, uniqueKey, ConsoleVoid, 2, 2,
   "Removes any elements that have duplicated keys (leaving the first instance)")
{
   object->uniqueKey();
}

ConsoleMethod(ArrayObject, duplicate, ConsoleBool, 3, 3,
   "Alters array into an exact duplicate of the target array.\n"
   "@param target ArrayObject to duplicate\n")
{
   SimObject *obj = Sim::findObject(argv[2]);
   if (!obj) return false;

   ArrayObject* target = dynamic_cast<ArrayObject*>(obj);
   if (!target) return false;

   if (target)
   {
      object->duplicate(target);
      return true;
   }

   return false;
}

ConsoleMethod(ArrayObject, crop, ConsoleBool, 3, 3,
   "Removes elements with matching keys from array.\n"
   "@param target ArrayObject containing keys to remove from this array\n")
{
   SimObject *obj = Sim::findObject(argv[2]);
   if (!obj) return false;

   ArrayObject* target = dynamic_cast<ArrayObject*>(obj);
   if (!target) return false;

   if (target)
   {
      object->crop(target);
      return true;
   }

   return false;
}

ConsoleMethod(ArrayObject, append, ConsoleBool, 3, 3,
   "Appends the target array to the array object.\n"
   "@param target ArrayObject to append to the end of this array\n")
{
   SimObject *obj = Sim::findObject(argv[2]);
   if (!obj) return false;

   ArrayObject* target = dynamic_cast<ArrayObject*>(obj);
   if (!target) return false;

   if (target)
   {
      object->append(target);
      return true;
   }

   return false;
}

ConsoleMethod(ArrayObject, sort, ConsoleVoid, 3, 3,
   "Alpha sorts the array by value\n\n"
   "@param ascending [optional] True for ascending sort, false for descending sort\n")
{
   object->sort(true, dAtob(argv[2]), false);
}

ConsoleMethod(ArrayObject, sorta, ConsoleVoid, 2, 2,
   "Alpha sorts the array by value in ascending order")
{
   object->sort(true, true, false);
}

ConsoleMethod(ArrayObject, sortd, ConsoleVoid, 2, 2,
   "Alpha sorts the array by value in descending order")
{
   object->sort(true, false, false);
}

ConsoleMethod(ArrayObject, sortk, ConsoleVoid, 3, 3,
   "Alpha sorts the array by key\n\n"
   "@param ascending [optional] True for ascending sort, false for descending sort\n")
{
   object->sort(false, dAtob(argv[2]), false);
}

ConsoleMethod(ArrayObject, sortka, ConsoleVoid, 2, 2,
   "Alpha sorts the array by key in ascending order")
{
   object->sort(false, true, false);
}

ConsoleMethod(ArrayObject, sortkd, ConsoleVoid, 2, 2,
   "Alpha sorts the array by key in descending order")
{
   object->sort(false, false, false);
}

ConsoleMethod(ArrayObject, sortn, ConsoleVoid, 3, 3,
   "Numerically sorts the array by value\n\n"
   "@param ascending [optional] True for ascending sort, false for descending sort\n")
{
   object->sort(true, dAtob(argv[2]), true);
}

ConsoleMethod(ArrayObject, sortna, ConsoleVoid, 2, 2,
   "Numerically sorts the array by value in ascending order")
{
   object->sort(true, true, true);
}

ConsoleMethod(ArrayObject, sortnd, ConsoleVoid, 2, 2,
   "Numerically sorts the array by value in descending order")
{
   object->sort(true, false, true);
}

ConsoleMethod(ArrayObject, sortnk, ConsoleVoid, 3, 3,
   "Numerically sorts the array by key\n\n"
   "@param ascending [optional] True for ascending sort, false for descending sort\n")
{
   object->sort(false, dAtob(argv[2]), true);
}

ConsoleMethod(ArrayObject, sortnka, ConsoleVoid, 2, 2,
   "Numerical sorts the array by key in ascending order")
{
   object->sort(false, true, true);
}

ConsoleMethod(ArrayObject, sortnkd, ConsoleVoid, 2, 2,
   "Numerical sorts the array by key in descending order")
{
   object->sort(false, false, true);
}

ConsoleMethod(ArrayObject, sortf, ConsoleVoid, 3, 3,
   "Sorts the array by value in ascending order using the given callback function.\n"
   "@param functionName Name of a function that takes two arguments A and B and returns -1 if A is less, 1 if B is less, and 0 if both are equal.\n\n"
   "@tsexample\n"
   "function mySortCallback(%a, %b)\n"
   "{\n"
   "   return strcmp( %a.name, %b.name );\n"
   "}\n\n"
   "%array.sortf( \"mySortCallback\" );\n"
   "@endtsexample\n")
{
   object->sort(true, true, argv[2]);
}

ConsoleMethod(ArrayObject, sortfk, ConsoleVoid, 3, 3,
   "Sorts the array by key in ascending order using the given callback function.\n"
   "@param functionName Name of a function that takes two arguments A and B and returns -1 if A is less, 1 if B is less, and 0 if both are equal."
   "@see sortf\n")
{
   object->sort(false, true, argv[2]);
}

ConsoleMethod(ArrayObject, sortfd, ConsoleVoid, 3, 3,
   "Sorts the array by value in descending order using the given callback function.\n"
   "@param functionName Name of a function that takes two arguments A and B and returns -1 if A is less, 1 if B is less, and 0 if both are equal."
   "@see sortf\n")
{
   object->sort(true, false, argv[2]);
}

ConsoleMethod(ArrayObject, sortfkd, ConsoleVoid, 3, 3,
   "Sorts the array by key in descending order using the given callback function.\n"
   "@param functionName Name of a function that takes two arguments A and B and returns -1 if A is less, 1 if B is less, and 0 if both are equal."
   "@see sortf\n")
{
   object->sort(false, false, argv[2]);
}

ConsoleMethod(ArrayObject, moveFirst, ConsoleInt, 2, 2,
   "Moves array pointer to start of array\n\n"
   "@return Returns the new array pointer")
{
   return object->moveFirst();
}

ConsoleMethod(ArrayObject, moveLast, ConsoleInt, 2, 2,
   "Moves array pointer to end of array\n\n"
   "@return Returns the new array pointer")
{
   return object->moveLast();
}

ConsoleMethod(ArrayObject, moveNext, ConsoleInt, 2, 2,
   "Moves array pointer to next position\n\n"
   "@return Returns the new array pointer, or -1 if already at the end")
{
   return object->moveNext();
}

ConsoleMethod(ArrayObject, movePrev, ConsoleInt, 2, 2,
   "Moves array pointer to prev position\n\n"
   "@return Returns the new array pointer, or -1 if already at the start")
{
   return object->movePrev();
}

ConsoleMethod(ArrayObject, getCurrent, ConsoleInt, 2, 2,
   "Gets the current pointer index")
{
   return object->getCurrent();
}

ConsoleMethod(ArrayObject, setCurrent, ConsoleVoid, 3, 3,
   "Sets the current pointer index.\n"
   "@param index New 0-based pointer index\n")
{
   object->setCurrent(dAtoi(argv[2]));
}

ConsoleMethod(ArrayObject, echo, ConsoleVoid, 2, 2,
   "Echos the array contents to the console")
{
   object->echo();
}