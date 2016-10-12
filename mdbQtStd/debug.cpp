#include "debug.h"

#define TIM_FORM	"hh:mm:ss.zzz"


/* ======================================================================== */
/*                              messageHandler                              */
/* ======================================================================== */
/* in main.cpp:
 *  qInstallMessageHandler(messageHandler);
 */
void mdMessageHandler(QtMsgType type, const QMessageLogContext &context,
							 const QString &msg) {
	Q_UNUSED(context);
	static QString lastDbgMsg;

	QByteArray localMsg = QString(msg).toLocal8Bit();

#ifdef PRINT_TIMESTAMP
	QByteArray localTime = QString("\033[38;5;240m[%s]\033[0m ")
								  .arg(QTime::currentTime().toString(TIM_FORM).toLocal8Bit());
#else
	QByteArray localTime = "";
#endif

	if (lastDbgMsg != msg) {
		lastDbgMsg = msg;
		switch (type) {
			case QtDebugMsg:
				fprintf(stderr, "\n\033[48;5;6mDEBG\033[0m %s%s ",
						  localTime.constData(), localMsg.constData());
				break;

#if QT_VERSION >= 0x050500
			case QtInfoMsg:
				fprintf(stderr, "\n\033[48;5;10mINFO\033[0m %s%s ",
						  localTime.constData(), localMsg.constData());
				break;
#endif
			case QtWarningMsg:
				fprintf(stderr, "\n\033[48;5;11mWARN\033[0m %s%s ",
						  localTime.constData(), localMsg.constData());
				break;

			case QtCriticalMsg:
				fprintf(stderr, "\n\033[48;5;196mCRITICAL\033[0m %s%s ",
						  localTime.constData(), localMsg.constData());
				break;

			case QtFatalMsg:
				fprintf(stderr, "\n\033[48;5;196mFATAL\033[0m %s%s ",
						  localTime.constData(), localMsg.constData());
				abort();
			default:
				fprintf(stderr, "UNKNOWN MESSAGE TYPE");
		}
	}
	else
		fprintf(stderr, " \033[48;5;11mR\033[0m");
}



struct TicToc::type_t TicToc::t = { QString(), QTime()};
struct TicToc::type_t2 *TicToc::t2 = { new TicToc::type_t2()};

QString stuff(QString s, int length, QChar sc = ' ') {
	while (s.length() < length)
		s += sc;

	while (s.length() >= length)
		s.remove(s.length() - 1, 1);

	return s;
}

// ###############################################################################################################
// # Find nonprintable|none-printable|control chars by grep and display non-printables via cat		( 2016-10-12	02:28:03 )
// ###############################################################################################################
// find . -iname 'mdtabview\.h' -exec grep '[[:cntrl:]]' {} \; | cat -v -e -t
// ###############################################################################################################
// # Find the return value and methode/classname of a C/C++ project folder 		( 2016-10-12	02:25:44 )
// ###############################################################################################################
// arr=( $(find . -iname '*\.h' -exec grep -oE '^\s\w+ \w+' {} \; | awk '{print$1}' | sort -h | uniq) )
// echo $(for ((i=0; i<${#arr[@]}; i++)); do echo ${#arr[i]}; done) | tr ' ' '\n' | sort -h | uniq
// ###############################################################################################################
// # Find the length of all class names of a C/C++ project folder 		( 2016-10-12	01:25:44 )
// ###############################################################################################################
// cd ~/CODES_local/qt_creator/worktimeManagerSubdirPrj/WorktimeManager
// arr=( $(find . -iname '*\.h' -exec grep -oE '^class [^Q^\;\w]+' {} \; | awk '{print$2}' | uniq ) )
// echo $(for ((i=0; i<${#arr[@]}; i++)); do echo ${#arr[i]}; done) | tr ' ' '\n' | sort -h | uniq

#define	RETURN_TYPE_LENGTH	12	//17
#define	CLASS_NAME_LENGTH		19
#define	METHODE_NAME_LENGTH	25 //32

/* ======================================================================== */
/*                              dbgFuncNameMdb                              */
/* ======================================================================== */
QString dbgFuncNameMdb(QString name) {
	QString nameNoArg = name.mid(0, name.indexOf(QChar('(')));

	QString output, outParam;
	QStringList classParts = name.mid(0, name.indexOf(QChar('('))).split("::");
	QStringList nameAndReturnType = classParts.first().split(" ");

	QString returnType = ""; //ctor, dtor don't have return types
	if(nameAndReturnType.count() > 1)
		returnType = nameAndReturnType.first() + " ";
	QString className = nameAndReturnType.last();

	QStringList funcAndParamas = classParts.last().split("(");
	//	funcAndParamas.last().chop(1);
	QString functionName = funcAndParamas.first();
	QStringList params = funcAndParamas.last().split(",");

	typedef struct {
		QString
				c1a, returnType, c1b,
				c2a, className,	c2b,
				c3a, methodeName,c3b;
	} o_t;

	o_t o;

	if (! returnType.isEmpty()) {
		o.c1a			= "\033[036m";			// 0
		o.returnType	= returnType;				// 1
		o.c1b			= "\033[0m";
	}
	else {
		o.c1a			= "\033[51m";			// 0
		o.returnType= QString("CTOR");				// 1
		o.c1b			= "\033[0m";
	}
	o.c2a			= "\033[32m";
	o.className	= className + QString("::");				// 3
	o.c2b			= "\033[0m  ";
	o.c3a			= "\033[34m";
	o.methodeName = functionName;			// 6
	o.c3b			= "\033[0m ";
	QStringList outl;

	QStringList::const_iterator param;
	for (param = params.begin(); param != params.constEnd(); ++param) {
		if(param != params.begin()) {
			outl << "\033[0m,";
		}
		outl << "\033[036m";
		outl << (*param);
	}
	outl << "\033[0m)";

	output.append(o.c1a + stuff(o.returnType, RETURN_TYPE_LENGTH) + o.c1b +
					  o.c2a + stuff(o.className, CLASS_NAME_LENGTH) + o.c2b +
					  o.c3a + stuff(o.methodeName, METHODE_NAME_LENGTH) + o.c3b + " ");

	return output;
}
