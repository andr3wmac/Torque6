//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/arrayObject.h"
#include "console/consoleTypes.h"
#include "math/mMathFn.h"

#include "arrayObject_Binding.h"

IMPLEMENT_CONOBJECT(ArrayObject);

bool ArrayObject::smDecreasing = false;
bool ArrayObject::smCaseSensitive = false;
const char* ArrayObject::smCompareFunction;


S32 QSORT_CALLBACK ArrayObject::_valueCompare(const void* a, const void* b)
{
   ArrayObject::Element *ea = (ArrayObject::Element *) (a);
   ArrayObject::Element *eb = (ArrayObject::Element *) (b);
   S32 result = smCaseSensitive ? dStrcmp(ea->value, eb->value) : dStricmp(ea->value, eb->value);
   return (smDecreasing ? -result : result);
}

S32 QSORT_CALLBACK ArrayObject::_valueNumCompare(const void* a, const void* b)
{
   ArrayObject::Element *ea = (ArrayObject::Element *) (a);
   ArrayObject::Element *eb = (ArrayObject::Element *) (b);
   F32 aCol = dAtof(ea->value);
   F32 bCol = dAtof(eb->value);
   F32 result = aCol - bCol;
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return (smDecreasing ? -res : res);
}

S32 QSORT_CALLBACK ArrayObject::_keyCompare(const void* a, const void* b)
{
   ArrayObject::Element *ea = (ArrayObject::Element *) (a);
   ArrayObject::Element *eb = (ArrayObject::Element *) (b);
   S32 result = smCaseSensitive ? dStrcmp(ea->value, eb->value) : dStricmp(ea->value, eb->value);
   return (smDecreasing ? -result : result);
}

S32 QSORT_CALLBACK ArrayObject::_keyNumCompare(const void* a, const void* b)
{
   ArrayObject::Element *ea = (ArrayObject::Element *) (a);
   ArrayObject::Element *eb = (ArrayObject::Element *) (b);
   const char* aCol = ea->key;
   const char* bCol = eb->key;
   F32 result = dAtof(aCol) - dAtof(bCol);
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return (smDecreasing ? -res : res);
}

S32 QSORT_CALLBACK ArrayObject::_keyFunctionCompare(const void* a, const void* b)
{
   ArrayObject::Element* ea = (ArrayObject::Element*)(a);
   ArrayObject::Element* eb = (ArrayObject::Element*)(b);

   S32 result = dAtoi(Con::executef(3, (const char*)smCompareFunction, ea->key, eb->key));
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return (smDecreasing ? -res : res);
}

S32 QSORT_CALLBACK ArrayObject::_valueFunctionCompare(const void* a, const void* b)
{
   ArrayObject::Element* ea = (ArrayObject::Element*)(a);
   ArrayObject::Element* eb = (ArrayObject::Element*)(b);

   S32 result = dAtoi(Con::executef(3, (const char*)smCompareFunction, ea->value, eb->value));
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return (smDecreasing ? -res : res);
}


//-----------------------------------------------------------------------------

ArrayObject::ArrayObject()
   : mCurrentIndex(NULL),
   mCaseSensitive(false)
{
}

//-----------------------------------------------------------------------------

void ArrayObject::initPersistFields()
{
   addField("caseSensitive", TypeBool, Offset(mCaseSensitive, ArrayObject),
      "Makes the keys and values case-sensitive.\n"
      "By default, comparison of key and value strings will be case-insensitive.");

   //addProtectedField("key", TypeCaseString, NULL, &_addKeyFromField, &defaultProtectedGetFn,
   //   "Helper field which allows you to add new key['keyname'] = value pairs.");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

bool ArrayObject::_addKeyFromField(void *object, const char *index, const char *data)
{
   static_cast<ArrayObject*>(object)->push_back(index, data);
   return false;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::getIndexFromValue(StringTableEntry value) const
{
   S32 foundIndex = -1;
   for (S32 i = mCurrentIndex; i < mArray.size(); i++)
   {
      if (isEqual(mArray[i].value, value))
      {
         foundIndex = i;
         break;
      }
   }

   if (foundIndex < 0)
   {
      for (S32 i = 0; i < mCurrentIndex; i++)
      {
         if (isEqual(mArray[i].value, value))
         {
            foundIndex = i;
            break;
         }
      }
   }

   return foundIndex;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::getIndexFromKey(StringTableEntry key) const
{
   S32 foundIndex = -1;
   for (S32 i = mCurrentIndex; i < mArray.size(); i++)
   {
      if (isEqual(mArray[i].key, key))
      {
         foundIndex = i;
         break;
      }
   }

   if (foundIndex < 0)
   {
      for (S32 i = 0; i < mCurrentIndex; i++)
      {
         if (isEqual(mArray[i].key, key))
         {
            foundIndex = i;
            break;
         }
      }
   }

   return foundIndex;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::getIndexFromKeyValue(StringTableEntry key, StringTableEntry value) const
{
   S32 foundIndex = -1;
   for (S32 i = mCurrentIndex; i < mArray.size(); i++)
   {
      if (isEqual(mArray[i].key, key) && isEqual(mArray[i].value, value))
      {
         foundIndex = i;
         break;
      }
   }

   if (foundIndex < 0)
   {
      for (S32 i = 0; i < mCurrentIndex; i++)
      {
         if (isEqual(mArray[i].key, key) && isEqual(mArray[i].value, value))
         {
            foundIndex = i;
            break;
         }
      }
   }

   return foundIndex;
}

//-----------------------------------------------------------------------------

StringTableEntry ArrayObject::getKeyFromIndex(S32 index) const
{
   if (index >= mArray.size() || index < 0)
      return StringTable->EmptyString;

   return mArray[index].key;
}

//-----------------------------------------------------------------------------

StringTableEntry ArrayObject::getValueFromIndex(S32 index) const
{
   if (index >= mArray.size() || index < 0)
      return StringTable->EmptyString;

   return mArray[index].value;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::countValue(StringTableEntry value) const
{
   S32 count = 0;
   for (S32 i = 0; i < mArray.size(); i++)
   {
      if (isEqual(mArray[i].value, value))
         count++;
   }

   return count;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::countKey(StringTableEntry key) const
{
   S32 count = 0;
   for (S32 i = 0; i < mArray.size(); i++)
   {
      if (isEqual(mArray[i].key, key))
         count++;
   }

   return count;
}

//-----------------------------------------------------------------------------

void ArrayObject::push_back(StringTableEntry key, StringTableEntry value)
{
   mArray.push_back(Element(key, value));
}

void ArrayObject::push_back(StringTableEntry value)
{
   char cStrKey[16];
   dSprintf(cStrKey, 16, "%d", mArray.size());
   mArray.push_back(Element(StringTable->insert(cStrKey), value));
}

//-----------------------------------------------------------------------------

void ArrayObject::push_front(StringTableEntry key, StringTableEntry value)
{
   mArray.push_front(Element(key, value));
}

//-----------------------------------------------------------------------------

void ArrayObject::insert(StringTableEntry key, StringTableEntry value, S32 index)
{
   index = mClamp(index, 0, mArray.size());
   mArray.insert(index, Element(key, value));
}

//-----------------------------------------------------------------------------

void ArrayObject::pop_back()
{
   if (mArray.size() <= 0)
      return;

   mArray.pop_back();

   if (mCurrentIndex >= mArray.size())
      mCurrentIndex = mArray.size() - 1;
}

//-----------------------------------------------------------------------------

void ArrayObject::pop_front()
{
   if (mArray.size() <= 0)
      return;

   mArray.pop_front();

   if (mCurrentIndex >= mArray.size())
      mCurrentIndex = mArray.size() - 1;
}

//-----------------------------------------------------------------------------

void ArrayObject::erase(S32 index)
{
   if (index < 0 || index >= mArray.size())
      return;

   mArray.erase(index);
}

//-----------------------------------------------------------------------------

void ArrayObject::empty()
{
   mArray.clear();
   mCurrentIndex = 0;
}

//-----------------------------------------------------------------------------

void ArrayObject::moveIndex(S32 prev, S32 index)
{
   if (index >= mArray.size())
      push_back(mArray[prev].key, mArray[prev].value);
   else
      mArray[index] = mArray[prev];
   mArray[prev].value = StringTable->EmptyString;
   mArray[prev].key = StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

void ArrayObject::uniqueValue()
{
   for (S32 i = 0; i<mArray.size(); i++)
   {
      for (S32 j = i + 1; j<mArray.size(); j++)
      {
         if (isEqual(mArray[i].value, mArray[j].value))
         {
            erase(j);
            j--;
         }
      }
   }
}

//-----------------------------------------------------------------------------

void ArrayObject::uniqueKey()
{
   for (S32 i = 0; i<mArray.size(); i++)
   {
      for (S32 j = i + 1; j<mArray.size(); j++)
      {
         if (isEqual(mArray[i].key, mArray[j].key))
         {
            erase(j);
            j--;
         }
      }
   }
}

//-----------------------------------------------------------------------------

void ArrayObject::duplicate(ArrayObject* obj)
{
   empty();
   for (S32 i = 0; i<obj->count(); i++)
   {
      StringTableEntry tempval = obj->getValueFromIndex(i);
      StringTableEntry tempkey = obj->getKeyFromIndex(i);
      push_back(tempkey, tempval);
   }
   mCurrentIndex = obj->getCurrent();
}

//-----------------------------------------------------------------------------

void ArrayObject::crop(ArrayObject *obj)
{
   for (S32 i = 0; i < obj->count(); i++)
   {
      StringTableEntry tempkey = obj->getKeyFromIndex(i);
      for (S32 j = 0; j < mArray.size(); j++)
      {
         if (isEqual(mArray[j].key, tempkey))
         {
            mArray.erase(j);
            j--;
         }
      }
   }
}

//-----------------------------------------------------------------------------

void ArrayObject::append(ArrayObject* obj)
{
   for (S32 i = 0; i<obj->count(); i++)
   {
      StringTableEntry tempval = obj->getValueFromIndex(i);
      StringTableEntry tempkey = obj->getKeyFromIndex(i);
      push_back(tempkey, tempval);
   }
}

//-----------------------------------------------------------------------------

void ArrayObject::setKey(StringTableEntry key, S32 index)
{
   if (index >= mArray.size())
      return;

   mArray[index].key = key;
}

//-----------------------------------------------------------------------------

void ArrayObject::setValue(StringTableEntry value, S32 index)
{
   if (index >= mArray.size())
      return;

   mArray[index].value = value;
}

//-----------------------------------------------------------------------------

void ArrayObject::sort(bool valsort, bool asc, bool numeric)
{
   if (mArray.size() <= 1)
      return;

   smDecreasing = asc ? false : true;
   smCaseSensitive = isCaseSensitive();

   if (numeric)
   {
      if (valsort)
         dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), _valueNumCompare);
      else
         dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), _keyNumCompare);
   }
   else
   {
      if (valsort)
         dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), _valueCompare);
      else
         dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), _keyCompare);
   }
}

//-----------------------------------------------------------------------------

void ArrayObject::sort(bool valsort, bool asc, const char* callbackFunctionName)
{
   if (mArray.size() <= 1)
      return;

   smDecreasing = asc ? false : true;
   smCompareFunction = callbackFunctionName;

   if (valsort)
      dQsort((void*)&(mArray[0]), mArray.size(), sizeof(Element), _valueFunctionCompare);
   else
      dQsort((void*)&(mArray[0]), mArray.size(), sizeof(Element), _keyFunctionCompare);

   smCompareFunction = NULL;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::moveFirst()
{
   mCurrentIndex = 0;
   return mCurrentIndex;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::moveLast()
{
   if (mArray.empty())
      mCurrentIndex = 0;
   else
      mCurrentIndex = mArray.size() - 1;
   return mCurrentIndex;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::moveNext()
{
   if (mCurrentIndex >= mArray.size() - 1)
      return -1;

   mCurrentIndex++;

   return mCurrentIndex;
}

//-----------------------------------------------------------------------------

S32 ArrayObject::movePrev()
{
   if (mCurrentIndex <= 0)
      return -1;

   mCurrentIndex--;

   return mCurrentIndex;
}

//-----------------------------------------------------------------------------

void ArrayObject::setCurrent(S32 idx)
{
   if (idx < 0 || idx >= mArray.size())
   {
      Con::errorf("ArrayObject::setCurrent( %d ) is out of the array bounds!", idx);
      return;
   }

   mCurrentIndex = idx;
}

//-----------------------------------------------------------------------------

void ArrayObject::echo()
{
   Con::printf("ArrayObject Listing:");
   Con::printf("Index   Key       Value");
   for (S32 i = 0; i < mArray.size(); i++)
   {
      StringTableEntry key = mArray[i].key;
      StringTableEntry val = mArray[i].value;
      Con::printf("%d      [%s]    =>    %s", i, key, val);
   }
}