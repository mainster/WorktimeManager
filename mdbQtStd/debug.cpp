#include "debug.h"

#define TIM_FORM	"hh:mm:ss.zzz"

void messageHandler(QtMsgType type,
						  const QMessageLogContext &context,
						  const QString &msg) {

	Q_UNUSED(context);

	QByteArray localMsg = msg.toLocal8Bit();
	QByteArray localTime = QTime::currentTime().toString(TIM_FORM).toLocal8Bit();

	if (type == QtDebugMsg)
		fprintf(stderr, "\033[48;5;6mDEBG\033[0m \033[38;5;240m[%s]\033[0m: %s\n",
				  localTime.constData(), localMsg.constData());

#if QT_VERSION >= 0x050500
	if (type == QtInfoMsg)
		fprintf(stderr, "\033[48;5;10mINFO\033[0m \033[38;5;240m[%s]\033[0m: %s\n",
				  localTime.constData(), localMsg.constData());
#endif
	if (type == QtWarningMsg)
		fprintf(stderr, "\033[48;5;11mWARN\033[0m \033[38;5;240m[%s]\033[0m: %s\n",
				  localTime.constData(), localMsg.constData());

	if (type == QtCriticalMsg)
		fprintf(stderr, "\033[48;5;196mCRITICAL\033[0m \033[38;5;240m[%s]\033[0m: %s\n",
				  localTime.constData(), localMsg.constData());

	if (type == QtFatalMsg)
		fprintf(stderr, "\033[48;5;196mFATAL\033[0m \033[38;5;240m[%s]\033[0m: %s\n",
				  localTime.constData(), localMsg.constData());
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

QString dbgFuncName(QString name) {

	QString output;
	QStringList classParts = name.split("::");
	QStringList nameAndReturnType = classParts.first().split(" ");

	QString returnType = ""; //ctor, dtor don't have return types
	if(nameAndReturnType.count() > 1)
		returnType = nameAndReturnType.first() + " ";
	QString className = nameAndReturnType.last();

	QStringList funcAndParamas = classParts.last().split("(");
	funcAndParamas.last().chop(1);
	QString functionName = funcAndParamas.first();
	QStringList params = funcAndParamas.last().split(",");

	output.append("\033[036m");
	output.append(returnType);
	output.append("\033[0m\033[32m");
	output.append(className);
	output.append("\033[0m::");
	output.append("\033[34m");
	output.append(functionName);
	output.append("\033[0m(");

	QStringList::const_iterator param;
	for (param = params.begin(); param != params.constEnd(); ++param) {
		if(param != params.begin()) {
			output.append("\033[0m,");
		}
		output.append("\033[036m");
		output.append((*param));
	}
	output.append("\033[0m)");
	return output;
}

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
	funcAndParamas.last().chop(1);
	QString functionName = funcAndParamas.first();
	QStringList params = funcAndParamas.last().split(",");

	typedef struct {
		QString
				c1a, returnType, c1b,
				c2a, className,	c2b,
				c3a, functionName,c3b;
	} o_t;

	o_t o;

	if (! returnType.isEmpty()) {
		o.c1a			= "\033[036m";			// 0
		o.returnType	= returnType;				// 1
		o.c1b			= "\033[0m";
	}
	else {
		o.c1a			= "\033[51m";			// 0
		o.returnType= QString("  CTOR");				// 1
		o.c1b			= "\033[0m";
	}
	o.c2a			= "\033[32m";
	o.className	= className + QString("::");				// 3
	o.c2b			= "\033[0m  ";
	o.c3a			= "\033[34m";
	o.functionName = functionName;			// 6
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

	output.append(o.c1a + stuff(o.returnType, 12) + o.c1b +
					  o.c2a + stuff(o.className, 12) + o.c2b +
					  o.c3a + stuff(o.functionName, 15) + o.c3b + " ");

	return output;
}
