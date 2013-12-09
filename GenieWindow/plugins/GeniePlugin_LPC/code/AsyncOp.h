#ifndef __AsyncOp_h__
#define __AsyncOp_h__

#include <QObject>
#include <QVariant>

class AsyncOpImpl;

class AsyncOp
	: public QObject
{
	Q_OBJECT

public:
	virtual ~AsyncOp();
	bool isAborted() const;
	QVariant result() const;

signals:
	void finished();

public slots:
	void abort();

protected:
	AsyncOp(QObject *parent);
	virtual void abortImpl();

	void report(const QVariant& result);

private:
	friend class AsyncOpImpl;
	AsyncOpImpl *m_impl;
};

#endif // __AsyncOp_h__
