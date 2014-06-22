#ifndef NEXTAR_JOBQUEUE_H
#define NEXTAR_JOBQUEUE_H

#include <NexBase.h>

namespace nextar {

typedef uint32 RequestID;

/**
 * @remarks The JobQueue is a unique queue handling jobs asynchronously.
 *	The queue is very generic in that it can handle any type of job requests
 *	and respond to after completing the requests. Listeners does the handling
 *	and responding.
 *	JobQueue may support thread pools.
 * @par The job queue is designed such that requests can be of any-type pertaining
 *	to action from any device be it the network, the gpu or the cpu. Once a new
 *	request is made the request listner should process it and return true if it
 *	was processed successfully. Upon completion of the request, the handler may
 *	place an appropriate response to that request. This response is then transf-
 *	erred to the 'wating on request' parties.
 *
 * @par     Feature Listing:
 * @par    - No starvation on jobs, suspend threads
 *         - Wait on requests
 *		   - Queue up requests until processing enabled
 **/
class JobQueue: public AllocGeneral {
public:

public:

	/**
	 * @brief Request listener.
	 **/
	struct _NexNoVtable RequestHandler {
		/**
		 * @brief Handle a request issued for this listener. This is not called from
		 *		  the main thread in most cases. So this call will reside in a back-
		 *		  ground thread and implementations needs to be thread safe.
		 **/
		virtual void AsyncHandleRequest(void* data) = 0;
	};

	/**
	 * @brief Request structure.
	 **/
	struct Request: public AllocGeneral {
		/* Identifier for the request assigned by the job queue */
		RequestHandler* handler;
		/* The data */
		void* data;

		Request() :
				handler(0), data(0) {
		}
	};

public:

	virtual ~JobQueue() {
	}

	/**
	 * @remarks Place a request in the job queue.
	 **/
	virtual void PlaceRequest(RequestHandler*, void*) = 0;

	/**
	 * @remarks Abort all requests for a certain listener
	 *  or for all listeners. **/
	virtual void AbortAllRequests() = 0;

	/** @remarks Will wait till the jobqueue shuts down */
	virtual void ShutDown() = 0;

private:
};

/**
 * @remarks Default implementation for job queue. This class implements
 *			pooled (threaded) job queue.
 **/
class GenericJobQueue: public JobQueue, public syncop::Runnable {

	typedef deque<Request>::type RequestQueue;

	NEX_LOG_HELPER (GenericJobQueue);

	bool quit;bool pause_processing;

	int32 request_count;
	RequestQueue process;

	NEX_THREAD_DEFINE_MUTEX (process_mtx);
	NEX_THREAD_DEFINE_SYNC_CONDITION (job_scheduler);

#ifdef NEX_MULTITHREADED
	NEX_THREAD_PTR_TYPE workers[4];
	size_t num_workers;
#endif
	class JQFrameListener: public FrameListener, public AllocGeneral {
	public:
		GenericJobQueue* jobQueue;

		JQFrameListener(GenericJobQueue* jq);
		~JQFrameListener();

		/* Called to mark the end of a frame, timeElapsed in milliseconds */
		virtual void EndFrame(uint32 timeElapsed);
	};

	friend class JQFrameListener;
	JQFrameListener* listener;

public:

	/**
	 * @remarks The number of worker specifies the number of worker threads. Only valid
	 *	if multi threading is enabled.
	 **/
	GenericJobQueue(size_t num_worker);
	/** @remarks Destructor **/
	virtual ~GenericJobQueue();

	/**
	 * @remarks Place a request in the job queue.
	 **/
	virtual void PlaceRequest(RequestHandler*, void*);

	/**
	 * @remarks Abort all requests for a certain listener or for all listeners.
	 **/
	virtual void AbortAllRequests();

	virtual void StopRequest();

	/** @copydoc syncop::Runnable::Run **/
	virtual void Run();

	/** @remarks Shuts down */
	virtual void ShutDown();

protected:

	Request GetPendingRequest();
};
}
;

#endif //NEXTAR_JOBQUEUE_H