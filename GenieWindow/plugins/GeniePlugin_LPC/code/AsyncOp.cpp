#include "AsyncOp.h"

class AsyncOpImpl
{
public:
	AsyncOpImpl();

	bool m_aborted;
	bool m_aborting;
	QVariant m_result;
};

AsyncOpImpl::AsyncOpImpl()
: m_aborted(false)
, m_aborting(false)
{
}

AsyncOp::AsyncOp(QObject *parent)
: QObject(parent)
{
	m_impl = new AsyncOpImpl();
}

AsyncOp::~AsyncOp()
{
	delete m_impl;
}

bool AsyncOp::isAborted() const
{
	return m_impl->m_aborted;
}

QVariant AsyncOp::result() const
{
	return m_impl->m_result;
}

void AsyncOp::abort()
{
	if (!m_impl->m_aborted) {
		m_impl->m_aborted = true;
		m_impl->m_aborting = true;
		abortImpl();
		m_impl->m_aborting = false;
	}
}

void AsyncOp::abortImpl()
{
}

void AsyncOp::report(const QVariant& result)
{
/*	if (isAborted()) {
	}
*/
	m_impl->m_result = result;

	if (!m_impl->m_aborting) {
		emit finished();
	}
}
