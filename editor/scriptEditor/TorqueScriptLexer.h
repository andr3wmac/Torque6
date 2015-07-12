// TorqueScript Scintilla Lexer by LukasPJ

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"
#include "OptionSet.h"
#include "SparseState.h"
#include "Catalogue.h"

#include "defsext.h"
#include "prefs.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

// Options used for LexerTorqueScript
struct OptionsTorqueScript {
	bool fold;
	bool foldCompact;
	OptionsTorqueScript() {
		fold = false;
		foldCompact = false;
	}
};

static const char *const TorqueScriptWordLists[] = {
            "Primary keywords and identifiers",
            "Secondary keywords and identifiers",
            "Documentation comment keywords",
            "Global classes and typedefs",
            "Preprocessor definitions",
            0,
};

struct OptionSetTorqueScript : public OptionSet<OptionsTorqueScript> {
	OptionSetTorqueScript() {
		DefineProperty("fold", &OptionsTorqueScript::fold);

		DefineProperty("fold.compact", &OptionsTorqueScript::foldCompact);
		DefineWordListSets(TorqueScriptWordLists);
	}
};

class LexerTorqueScript : public ILexer {
	bool caseSensitive;
	CharacterSet setWord;
   CharacterSet setBinOp;
   WordList declarationKeywords;
   WordList keywords;
   WordList stringcats;
	WordList keywords4;
	OptionsTorqueScript options;
	OptionSetTorqueScript osTorqueScript;
	SparseState<std::string> rawStringTerminators;
	enum { activeFlag = 0x40 };
   enum Token{
      VAR,
      BINOP,
      DECLARATION,
      KEYWORD,
      LITERAL,
      IDENTIFIER,
      UNDEFINED
   };
public:
   LexerTorqueScript() :
      setWord(CharacterSet::setAlphaNum, "_:", 0x80, true),
      setBinOp(CharacterSet::setNone, "=+-*/%&|<>") {
	}
	virtual ~LexerTorqueScript() {
	}
	void SCI_METHOD Release() {
		delete this;
	}
	int SCI_METHOD Version() const {
		return lvOriginal;
	}
	const char * SCI_METHOD PropertyNames() {
		return osTorqueScript.PropertyNames();
	}
	int SCI_METHOD PropertyType(const char *name) {
		return osTorqueScript.PropertyType(name);
	}
	const char * SCI_METHOD DescribeProperty(const char *name) {
		return osTorqueScript.DescribeProperty(name);
	}
	int SCI_METHOD PropertySet(const char *key, const char *val);
	const char * SCI_METHOD DescribeWordListSets() {
		return osTorqueScript.DescribeWordListSets();
	}
	int SCI_METHOD WordListSet(int n, const char *wl);
	void SCI_METHOD Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess);
	void SCI_METHOD Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess);

	void * SCI_METHOD PrivateCall(int, void *) {
		return 0;
	}

	static ILexer *LexerFactoryTorqueScript() {
		return new LexerTorqueScript();
	}
	static int MaskActive(int style) {
		return style & ~activeFlag;
	}
   bool TSWordlistContainsKey(WordList &list, int length, LexAccessor &styler, int i);
   int TraceToEnd(int i, LexAccessor &styler, CharacterSet charSet);
   int FindMatching(int i, LexAccessor &styler, char character, bool singleLine = true);
   int FindFirst(int i, LexAccessor &styler, CharacterSet findSet, CharacterSet ignoreSet, bool singleLine = true);
   Token FindPreviousToken(int i, LexAccessor &styler);
};

extern LexerModule lmTorqueScript;