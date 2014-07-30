#include <NexBase.h>
#include <BackgroundStreamer.h>
#include <BackgroundStreamerImpl.h>
#include <ApplicationContext.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(BackgroundStreamer);

/************************************************************************/
/* BackgroundStreamer                                                   */
/************************************************************************/
BackgroundStreamer ::BackgroundStreamer() {
}

BackgroundStreamer::~BackgroundStreamer() {

}

/************************************************************************/
/* BackgroundStreamerImpl                                               */
/************************************************************************/
BackgroundStreamerImpl::BackgroundStreamerImpl() {
	for (auto& t : streamingThreads) {
		t = std::thread(&BackgroundStreamerImpl::ExecutePoolRequests, this);
	}

	ApplicationContext::Listener l(this, ApplicationContext::PRIORITY_LOW);
	ApplicationContext::Instance().RegisterListener(l);
}

BackgroundStreamerImpl::~BackgroundStreamerImpl() {
	{
		std::lock_guard<std::mutex> g(requestQueueLock);
		quitThreads = true;
	}
	requestQueueVar.notify_all();
	for (auto& t : streamingThreads) {
		t.join();
	}
	ApplicationContext::Listener l(this, ApplicationContext::PRIORITY_LOW);
	ApplicationContext::Instance().UnregisterListener(l);
}

void BackgroundStreamerImpl::AddRequest(StreamRequest* streamReq) {
	{
		std::lock_guard<std::mutex> g(requestQueueLock);
		requestQueue.push_back(streamReq);

	}
	requestQueueVar.notify_one();
}

void BackgroundStreamerImpl::ExecutePoolRequests() {
	while (true) {
		StreamRequest* request = WaitAndPopRequest();
		if (!request)
			return;
		bool result = true;
		try {
			if (request->flags & StreamRequest::REQUEST_SAVE)
				request->streamedObject->AsyncSave(request);
			else
				request->streamedObject->AsyncLoad(request);
		} catch (GracefulErrorExcept& ge) {
			result = false;
			Warn(ge.GetMsg());
		}
		if (result) {
			// add to loaded
			AddResponse(request);
			// this might result in a call to process responses
			// even when there are none, but thats not an issue.
			responseProcessed.clear(std::memory_order_relaxed);
		}
	}
}

StreamRequest* BackgroundStreamerImpl::WaitAndPopRequest() {
	std::unique_lock<std::mutex> g(requestQueueLock);
	requestQueueVar.wait(g, [this] {
		return !requestQueue.empty() || quitThreads;
	});
	if (quitThreads)
		return nullptr;
	StreamRequest* ret = requestQueue.back();
	requestQueue.pop_back();
	return ret;
}

void BackgroundStreamerImpl::AddResponse(StreamRequest* request) {
	std::lock_guard<std::mutex> g(responseQueueLock);
	responseQueue.push_back(request);
	// this is the safest place to clear this flag
	// but might be an overkill as the lock will be active
	// for a long time.
	// responseProcessed.clear(std::memory_order_relaxed);
}

void BackgroundStreamerImpl::EndFrame(uint32 timeElapsed) {
	// @note We can control the frequency of calls here by the timeElapsed
	// parameter.
	if (!responseProcessed.test_and_set(std::memory_order_relaxed)) {
		std::lock_guard<std::mutex> g(responseQueueLock);
		for (auto& r : responseQueue) {
			if (r->flags & StreamRequest::REQUEST_SAVE)
				r->streamHandler->NotifySaved(r);
			else
				r->streamHandler->NotifyLoaded(r);
		}
	}
}
}

