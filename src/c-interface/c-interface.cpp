#include "c-interface.h"
#include "collection\vector.h"

CInterface& CInterface::GetCInterface()
{
   static CInterface INSTANCE;
   return INSTANCE;
}

bool CInterface::isMethod(const char* className, const char* methodName)
{
   return GetCInterface()._isMethod(className, methodName);
}

const char* CInterface::CallFunction(const char* name, const char **argv, int argc, bool *result)
{
   return GetCInterface()._CallFunction(name, argv, argc, result);
}

const char* CInterface::CallMethod(SimObject* obj, const char* name, const char **argv, int argc, bool *res)
{
   return GetCInterface()._CallMethod(obj->getClassName(), obj->getId(), name, argv, argc, res);
}

void CInterface::CallMain(bool *res)
{
   GetCInterface()._CallMain(res);
}

bool CInterface::_isMethod(const char* className, const char* methodName)
{
   if (mIsMethodCallback){
      return mIsMethodCallback(className, methodName);
   }
   return NULL;
}

const char* CInterface::_CallFunction(const char* name, const char **argv, int argc, bool *result)
{
   if (mFunctionCallback){
      return mFunctionCallback(name, argv, argc, result);
   }
   *result = false;
   return NULL;
}

const char* CInterface::_CallMethod(const char* className, U32 object, const char* name, const char **argv, int argc, bool *res)
{
   if (mMethodCallback){
      return mMethodCallback(className, object, name, argv, argc, res);
   }
   *res = false;
   return NULL;
}

void CInterface::_CallMain(bool *res)
{
   if (mMainCallback){
      *res = true;
      mMainCallback();
      return;
   }
   *res = false;
}

char* CInterface::GetMarshallableString(const char* str)
{
   if (str == NULL)
      return NULL;
   const ULONG ulSize = dStrlen(str) + sizeof(char);
   char* pszReturn = (char*)::CoTaskMemAlloc(ulSize);
   // Copy the contents of szSampleString
   // to the memory pointed to by pszReturn.
   strcpy(pszReturn, str);
   // Return pszReturn.
   return pszReturn;
}

char* CInterface::GetMarshallableString(int size)
{
   const ULONG ulSize = size + sizeof(char);
   char* pszReturn = (char*)::CoTaskMemAlloc(ulSize);
   // Return pszReturn.
   return pszReturn;
}

void SetCallbacks(void* ptr, void* methodPtr, void* isMethodPtr, void* mainPtr) {
   CInterface::GetCInterface().SetCallFunctionCallback(ptr);
   CInterface::GetCInterface().SetCallMethodCallback(methodPtr);
   CInterface::GetCInterface().SetCallIsMethodCallback(isMethodPtr);
   CInterface::GetCInterface().SetMainCallback(mainPtr);
}