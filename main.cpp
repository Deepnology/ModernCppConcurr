#include "NewAlgorithm.h"
#include "ThreadSafeLinkedList.h"
#include "DotProductAsync.h"
#include "PackagedTask.h"
#include "SharedFuture.h"
#include "PromiseFuture.h"
#include "PromiseFutureException.h"
#include "PromiseFutureSynchronize.h"
#include "AtomicMemModelCase1.h"
#include "AtomicMemModelCase2.h"
#include "AcquireReleaseWithWaiting.h"
#include "AcquireReleaseSpinlock.h"
#include "AcquireReleaseTransitivity.h"
#include "AcquireReleaseConsumerProducer.h"
#include "AcquireReleaseFenceConsumerProducer.h"
#include "ConsumeReleaseConsumerProducer.h"
#include "DeadlockResolved.h"
#include "Singleton.h"
#include "ThreadLocal.h"
#include "FireAndForgetFutures.h"
#include "MultithreadedSummation.h"
#include "AtomicStack.h"
int main()
{
    //NewAlgorithm::Run();
    //ThreadSafeLinkedList::Run();
    //DotProductAsync::Run();
    //PackagedTask::Run();
    //SharedFuture::Run();
    //PromiseFuture::Run();
    //PromiseFutureException::Run();
    //PromiseFutureSynchronize::Run();
    //AtomicMemModelCase1::sequential_consistency::Run();
    //for (int i = 0; i < 100; ++i)
    //AtomicMemModelCase1::acquire_release::Run();
    //AtomicMemModelCase1::relaxed::Run();

    //AtomicMemModelCase2::sequential_consistency::Run();
    //for (int i = 0; i < 100; ++i)
    //AtomicMemModelCase2::acquire_release::Run();
    //AtomicMemModelCase2::relaxed::Run();
    //AcquireReleaseWithWaiting::Run();
    //AcquireReleaseSpinlock::Run();

    //AcquireReleaseTransitivity::Run();

    //AcquireReleaseConsumerProducer::Run();
    //AcquireReleaseFenceConsumerProducer::Run();
    //ConsumeReleaseConsumerProducer::Run();

    //DeadlockResolved::Run();

    //Singleton::Run();

    //ThreadLocal::Run();

    //FireAndForgetFutures::Run();

    //MultithreadedSummation::Run();

    //atomic_stack::Run();
}