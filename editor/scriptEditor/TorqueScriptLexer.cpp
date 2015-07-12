// TorqueScript Scintilla Lexer by LukasPJ

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#include "TorqueScriptLexer.h"

LexerModule lmTorqueScript(SCLEX_TORQUESCRIPT, LexerTorqueScript::LexerFactoryTorqueScript, "TorqueScript", TorqueScriptWordLists);

int SCI_METHOD LexerTorqueScript::PropertySet(const char *key, const char *val) {
	if (osTorqueScript.PropertySet(&options, key, val)) {
		return 0;
	}
	return -1;
}

int SCI_METHOD LexerTorqueScript::WordListSet(int n, const char *wl) {
	WordList *wordListN = 0;
	switch (n) {
	case 0:
      wordListN = &keywords;
		break;
	case 1:
      wordListN = &declarationKeywords;
		break;
	case 2:
		wordListN = &stringcats;
		break;
	case 3:
		wordListN = &keywords4;
		break;
	}
	int firstModification = -1;
	if (wordListN) {
		WordList wlNew;
		wlNew.Set(wl);
		if (*wordListN != wlNew) {
			wordListN->Set(wl);
			firstModification = 0;
		}
	}
	return firstModification;
}

bool LexerTorqueScript::TSWordlistContainsKey(WordList &list, int length, LexAccessor &styler, int i)
{
   for (int jj = 0; jj < list.len; jj++)
      if (strlen(list.words[jj]) == length)
         if (styler.Match(i, list.words[jj]))
            return true;
   return false;
}

int LexerTorqueScript::TraceToEnd(int i, LexAccessor &styler, CharacterSet charSet)
{
   int end = i;
   char tstChar = styler.SafeGetCharAt(end + 1, '\0');
   while (charSet.Contains(tstChar))
   {
      end++;
      tstChar = styler.SafeGetCharAt(end + 1, '\0');
   }
   return end;
}

int LexerTorqueScript::FindMatching(int i, LexAccessor &styler, char character, bool singleLine)
{
   int end = i + 1;
   char tstChar = styler.SafeGetCharAt(end, '\0');
   while (tstChar != character
      && tstChar != '\0')
   {
      if (singleLine && (tstChar == '\r' || tstChar == '\n'))
         return -1;
      end++;
      tstChar = styler.SafeGetCharAt(end, '\0');
   }
   return end;
}

int LexerTorqueScript::FindFirst(int i, LexAccessor &styler, CharacterSet findSet, CharacterSet ignoreSet, bool singleLine)
{
   int end = i + 1;
   char tstChar = styler.SafeGetCharAt(end, '\0');
   while (!findSet.Contains(tstChar))
   {
      if (singleLine && (tstChar == '\r' || tstChar == '\n')
         || !ignoreSet.Contains(tstChar)
         || tstChar == '\0')
         return -1;
      end++;
      tstChar = styler.SafeGetCharAt(end, '\0');
   }
   return end;
}

LexerTorqueScript::Token LexerTorqueScript::FindPreviousToken(int i, LexAccessor &styler)
{
   Token result = Token::UNDEFINED;
   while (i >= 0 && result == Token::UNDEFINED)
   {
      int state = styler.StyleAt(i);
      switch (state)
      {
      case SCE_TORQUESCRIPT_BINOP:
      case SCE_TORQUESCRIPT_STRINGCAT:
         result = Token::BINOP;
         break;
      case SCE_TORQUESCRIPT_STRING:
      case SCE_TORQUESCRIPT_NUMBER:
         result = Token::LITERAL;
         break;
      case SCE_TORQUESCRIPT_LOCALVAR:
      case SCE_TORQUESCRIPT_GLOBALVAR:
         result = Token::VAR;
         break;
      case SCE_TORQUESCRIPT_IDENTIFIER:
         result = Token::IDENTIFIER;
         break;
      case SCE_TORQUESCRIPT_KEYWORD:
         result = Token::KEYWORD;
         break;
      case SCE_TORQUESCRIPT_DECLARATIONKEYWORD:
         result = Token::DECLARATION;
         break;
      case SCE_TORQUESCRIPT_DELIMITER:
         return Token::UNDEFINED;
      }
      i--;
   }
   return result;
}

void SCI_METHOD LexerTorqueScript::Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
   LexAccessor styler(pAccess);

   CharacterSet setWordStart(CharacterSet::setAlpha, "_", 0x80, true);
   CharacterSet setDeclarationWord(CharacterSet::setAlpha, "_:", 0x80, true);
   CharacterSet setKeywords(CharacterSet::setAlphaNum, "_$");
   CharacterSet setNumbers(CharacterSet::setDigits, ".");
   CharacterSet setEqual(CharacterSet::setNone, "=");
   CharacterSet setColon(CharacterSet::setNone, ":");
   CharacterSet setWhiteSpace(CharacterSet::setNone, "\t ");

   Token previousToken = FindPreviousToken(startPos - 1, styler);

   styler.StartAt(startPos);

   int state = initStyle;
   char chNext = styler.SafeGetCharAt(startPos);
   styler.StartSegment(startPos);
   int lengthDoc = startPos + length;
   char chVerbatimDelim = '\0';

   for (int i = startPos; i < lengthDoc; i++) {
      char ch = chNext;
      chNext = styler.SafeGetCharAt(i + 1);

      if (styler.IsLeadByte(ch)) {
         i++;
         chNext = styler.SafeGetCharAt(i + 1);
         continue;
      }
      int end;
      switch (state) {
      case SCE_TORQUESCRIPT_DEFAULT:
         switch (ch){
         case '/':
            if (chNext == '/')
               state = SCE_TORQUESCRIPT_COMMENT;
            if (chNext == '*')
               state = SCE_TORQUESCRIPT_BLOCKCOMMENT;
            styler.ColourTo(i, state);
            break;
         case '%':
            end = TraceToEnd(i, styler, setWord);
            styler.StartSegment(i);
            if (previousToken == Token::DECLARATION
               || previousToken == Token::VAR)
               styler.ColourTo(end, SCE_TORQUESCRIPT_ERROR);
            else
               styler.ColourTo(end, SCE_TORQUESCRIPT_LOCALVAR);
            i = end;
            chNext = styler.SafeGetCharAt(i + 1);
            previousToken = Token::VAR;
            break;
         case '$':
            if (styler.SafeGetCharAt(i + 1) == '=')
            {
               styler.ColourTo(++i, SCE_TORQUESCRIPT_BINOP);
               chNext = styler.SafeGetCharAt(i + 1);
               previousToken = Token::BINOP;
               break;
            }
            end = TraceToEnd(i, styler, setWord);
            styler.StartSegment(i);
            if (previousToken == Token::DECLARATION
               || previousToken == Token::VAR)
               styler.ColourTo(end, SCE_TORQUESCRIPT_ERROR);
            else
               styler.ColourTo(end, SCE_TORQUESCRIPT_GLOBALVAR);
            i = end;
            chNext = styler.SafeGetCharAt(i + 1);
            previousToken = Token::VAR;
            break;
         case '"':
         case '\'':
            end = FindMatching(i, styler, ch);
            i = end;
            if (end < 0)
            {
               i = -end;
               styler.ColourTo(i, SCE_TORQUESCRIPT_ERROR);
            }
            else
            {
               int equal = FindFirst(end, styler, setColon, setWhiteSpace, false);
               if ((previousToken == Token::DECLARATION
                  || previousToken == Token::VAR
                  || previousToken == Token::IDENTIFIER
                  || previousToken == Token::KEYWORD
                  || previousToken == Token::LITERAL)
                  && equal < 0)
                  styler.ColourTo(i, SCE_TORQUESCRIPT_ERROR);
               else
                  styler.ColourTo(i, ch == '"' ? SCE_TORQUESCRIPT_STRING : SCE_TORQUESCRIPT_TAG);
            }
            previousToken = Token::LITERAL;
            chNext = styler.SafeGetCharAt(i + 1);
            break;
         case '\r':
         case '\n':
            styler.ColourTo(i, state);
            break;
         case ';':
         case ',':
         case '{':
         case '}':
         case '.':
         case '(':
         case ')':
         case '[':
         case ']':
            if (previousToken == Token::DECLARATION)
               styler.ColourTo(i, SCE_TORQUESCRIPT_ERROR);
            else
               styler.ColourTo(i, SCE_TORQUESCRIPT_DELIMITER);
            previousToken = Token::UNDEFINED;
            break;
         case '@':
            styler.ColourTo(i, SCE_TORQUESCRIPT_STRINGCAT);
            previousToken = Token::BINOP;
            break;
         default:
            if (setBinOp.Contains(ch))
            {
               if (previousToken == Token::DECLARATION
                  || previousToken == Token::BINOP)
               {
                  if ((ch == '+'
                     || ch == '-'
                     || ch == '&'
                     || ch == '|')
                     && styler.SafeGetCharAt(i - 1) == ch)
                     styler.ColourTo(i, SCE_TORQUESCRIPT_DEFAULT);
                  else
                     styler.ColourTo(i, SCE_TORQUESCRIPT_ERROR);
               }
               else
               {
                  if (styler.SafeGetCharAt(i + 1) == '=')
                  {
                     i++;
                     chNext = styler.SafeGetCharAt(i + 1);
                     previousToken = Token::BINOP;
                  }
                  styler.ColourTo(i, SCE_TORQUESCRIPT_BINOP);
               }
               previousToken = Token::BINOP;
               break;
            }
            if (setNumbers.Contains(ch))
            {
               end = TraceToEnd(i, styler, setNumbers);
               if (previousToken == Token::DECLARATION
                  || previousToken == Token::VAR
                  || previousToken == Token::IDENTIFIER
                  || previousToken == Token::LITERAL
                  || previousToken == Token::KEYWORD)
                  styler.ColourTo(i, SCE_TORQUESCRIPT_ERROR);
               else
                  styler.ColourTo(end, SCE_TORQUESCRIPT_NUMBER);
               previousToken = Token::LITERAL;
               i = end;
               chNext = styler.SafeGetCharAt(i + 1);
               break;
            }
            if (setWordStart.Contains(ch))
            {
               int initialI = i;
               end = TraceToEnd(i, styler, setKeywords);
               bool foundKeyword = false;
               if (TSWordlistContainsKey(declarationKeywords, (end - i) + 1, styler, i))
               {
                  if (previousToken == Token::DECLARATION
                     || previousToken == Token::VAR)
                     state = SCE_TORQUESCRIPT_ERROR;
                  else
                     state = SCE_TORQUESCRIPT_DECLARATIONKEYWORD;
                  foundKeyword = true;
                  previousToken = Token::DECLARATION;
               }
               if (TSWordlistContainsKey(keywords, (end - i) + 1, styler, i))
               {
                  if (previousToken == Token::DECLARATION)
                     state = SCE_TORQUESCRIPT_ERROR;
                  else
                     state = SCE_TORQUESCRIPT_KEYWORD;
                  foundKeyword = true;
                  previousToken = Token::KEYWORD;
               }
               if (TSWordlistContainsKey(stringcats, (end - i) + 1, styler, i))
               {
                  if (previousToken == Token::DECLARATION
                     || previousToken == Token::UNDEFINED)
                     state = SCE_TORQUESCRIPT_ERROR;
                  else
                     state = SCE_TORQUESCRIPT_STRINGCAT;
                  foundKeyword = true;
                  previousToken = Token::BINOP;
               }
               if (foundKeyword)
               {
                  int equal = FindFirst(end, styler, setEqual, setWhiteSpace, false);
                  if (equal >= 0)
                  {
                     state = SCE_TORQUESCRIPT_DEFAULT;
                     previousToken = Token::IDENTIFIER;
                  }
                  styler.ColourTo(end, state);
                  state = SCE_TORQUESCRIPT_DEFAULT;
                  i = end;
                  chNext = styler.SafeGetCharAt(i + 1);
                  break;
               }

               if (previousToken == Token::DECLARATION)
                  end = TraceToEnd(i, styler, setDeclarationWord);
               else
                  end = TraceToEnd(i, styler, setWord);

               styler.StartSegment(i);
               if (previousToken == Token::LITERAL
                  || previousToken == Token::VAR
                  || previousToken == Token::LITERAL
                  || previousToken == Token::IDENTIFIER)
                  styler.ColourTo(end, SCE_TORQUESCRIPT_ERROR);
               else
                  styler.ColourTo(end, SCE_TORQUESCRIPT_IDENTIFIER);
               i = end;
               chNext = styler.SafeGetCharAt(i + 1);
               previousToken = Token::IDENTIFIER;
               break;
            }
            styler.ColourTo(i, state);
            break;
         }
         break;
      case SCE_TORQUESCRIPT_COMMENT:
         switch (ch){
         case '\r':
         case '\n':
            state = SCE_TORQUESCRIPT_DEFAULT;
            styler.ColourTo(i, state);
            break;
         default:
            styler.ColourTo(i, state);
            break;
         }
         break;
      case SCE_TORQUESCRIPT_BLOCKCOMMENT:
         switch (ch){
         case '*':
            if (chNext == '/')
            {
               styler.ColourTo(++i, state);
               state = SCE_TORQUESCRIPT_DEFAULT;
            }
            break;
         default:
            styler.ColourTo(i, state);
            break;
         }
         break;
      case SCE_TORQUESCRIPT_IDENTIFIER:
         if (IsAlphaNumeric(ch)
            || ch == '_')
         {
            styler.ColourTo(i, state);
            break;
         }
         state = SCE_TORQUESCRIPT_DEFAULT;
         styler.ColourTo(i, state);
         break;
      }
   }
   styler.Flush();
}

// Store both the current line's fold level and the next lines in the
// level store to make it easy to pick up with each increment
// and to make it possible to fiddle the current level for "} else {".

void SCI_METHOD LexerTorqueScript::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {

   if (!options.fold)
      return;

   LexAccessor styler(pAccess);

   int curLine = styler.GetLine(startPos);
   int prevLevel = curLine - 1 >= 0 ? styler.LevelAt(curLine - 1) : SC_FOLDLEVELBASE;
   bool prevWasHeader = prevLevel & SC_FOLDLEVELHEADERFLAG > 0;
   int currLevel = prevLevel & SC_FOLDLEVELNUMBERMASK;
   if (prevWasHeader)
      currLevel++;
   int curLineStart = styler.LineStart(curLine);
   //int currLevel = curLine >= 0 ? styler.LevelAt(curLine) & SC_FOLDLEVELNUMBERMASK : SC_FOLDLEVELBASE;
   int nextLevel = currLevel;
   int i = curLineStart;

   do {
      int lineType = styler.StyleAt(curLineStart);

      int visChars = 0;
      while (styler.GetLine(i) == curLine && i < startPos + length)
      {
         char ch = styler.SafeGetCharAt(i);
         switch (ch)
         {
         case '{':
            nextLevel++;
            visChars++;
            break;
         case '}':
            currLevel--;
            nextLevel--;
            visChars++;
            break;
         case '\t':
         case ' ':
         case '\r':
         case '\n':
            break;
         default:
            visChars++;
         }
         i++;
      }

      int lev = currLevel;

      // Set the "all whitespace" bit if the line is blank.
      if (visChars == 0 && options.foldCompact)
         lev |= SC_FOLDLEVELWHITEFLAG;

      // Set the "header" bit if needed.
      if ((nextLevel > currLevel) && (visChars > 0))
         lev |= SC_FOLDLEVELHEADERFLAG;
      styler.SetLevel(curLine, lev);
      currLevel = nextLevel;

      curLineStart = styler.LineStart(++curLine);
   } while (static_cast<int>(startPos)+length > curLineStart);
   styler.Flush();
}