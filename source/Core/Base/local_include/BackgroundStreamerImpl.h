/*
 * BackgroundStreamer.h
 *
 *  Created on: 17-Sep-2013
 *      Author: obhi
 */

#ifndef BACKGROUNDSTREAMERIMPL_H_
#define BACKGROUNDSTREAMERIMPL_H_

#include <NexBase.h>
#include <BackgroundStreamer.h>
#include <NexThread.h>
#include <FrameListener.h>
#include <Logger.h>

namespace nextar {

	// to extend this class to support external streaming as well as component streaming
	// todo this implementation needs profiling
	class BackgroundStreamerImpl : 
		public BackgroundStreamer,
		public FrameListener {

		NEX_LOG_HELPER(BackgroundStreamerImpl);

	public:
		
		BackgroundStreamerImpl();
		virtual ~BackgroundStreamerImpl();

		virtual void AddRequest(StreamRequest*);
		
	protected:

		// thread entry point
		void ExecutePoolRequests();
		StreamRequest* WaitAndPopRequest();
		void AddResponse(StreamRequest* request);
				
		virtual void EndFrame(uint32 timeElapsed);



		typedef array<std::thread, (uint32)Constants::MAX_LOADER_THREAD>::type ThreadList;
		typedef vector<StreamRequest*>::type StreamRequestList;
		
		std::mutex requestQueueLock;
		std::mutex responseQueueLock;
		std::condition_variable requestQueueVar;
		std::atomic_flag responseProcessed;
		bool quitThreads;

		StreamRequestList requestQueue;
		StreamRequestList responseQueue;

		ThreadList streamingThreads;
	};

} /* namespace nextar */
#endif /* BACKGROUNDSTREAMERIMPL_H_ */
