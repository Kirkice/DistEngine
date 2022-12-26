//
// Created by Administrator on 2022/12/26.
//

#ifndef APPLICATION_ASSERT_H
#define APPLICATION_ASSERT_H

#pragma once

#define POW2_ASSERTS_ENABLED

namespace pow2 { namespace Assert
    {
        enum FailBehavior
        {
            Halt,
            Continue,
        };

        typedef FailBehavior (*Handler)(const char* condition,
                                        const char* msg,
                                        const char* file,
                                        int line);

        Handler GetHandler();
        void SetHandler(Handler newHandler);

        FailBehavior ReportFailure(const char* condition,
                                   const char* file,
                                   int line,
                                   const char* msg, ...);
    }}

#define POW2_HALT() __debugbreak()
#define POW2_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef POW2_ASSERTS_ENABLED
#define POW2_ASSERT(cond) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (pow2::Assert::ReportFailure(#cond, __FILE__, __LINE__, 0) == \
					pow2::Assert::Halt) \
					POW2_HALT(); \
			} \
		} while(0)

#define POW2_ASSERT_MSG(cond, msg, ...) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (pow2::Assert::ReportFailure(#cond, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
					pow2::Assert::Halt) \
					POW2_HALT(); \
			} \
		} while(0)

#define POW2_ASSERT_FAIL(msg, ...) \
		do \
		{ \
			if (pow2::Assert::ReportFailure(0, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
				pow2::Assert::Halt) \
			POW2_HALT(); \
		} while(0)

#define POW2_VERIFY(cond) POW2_ASSERT(cond)
#define POW2_VERIFY_MSG(cond, msg, ...) POW2_ASSERT_MSG(cond, msg, ##__VA_ARGS__)
#else
#define POW2_ASSERT(condition) \
		do { POW2_UNUSED(condition); } while(0)
	#define POW2_ASSERT_MSG(condition, msg, ...) \
		do { POW2_UNUSED(condition); POW2_UNUSED(msg); } while(0)
	#define POW2_ASSERT_FAIL(msg, ...) \
		do { POW2_UNUSED(msg); } while(0)
	#define POW2_VERIFY(cond) (void)(cond)
	#define POW2_VERIFY_MSG(cond, msg, ...) \
		do { (void)(cond); POW2_UNUSED(msg); } while(0)
#endif

// SampleFramework12 Asserts
#ifdef _DEBUG
#define UseAsserts_ 1
#else
#define UseAsserts_ 0
#endif

#if UseAsserts_
#define Assert_(x) POW2_ASSERT(x)
#define AssertMsg_(x, msg, ...) POW2_ASSERT_MSG(x, msg, __VA_ARGS__)
#define AssertFail_(msg, ...) POW2_ASSERT_FAIL(msg, __VA_ARGS__)
#else
#define Assert_(x)
    #define AssertMsg_(x, msg, ...)
    #define AssertFail_(msg, ...)
#endif

#define UseStaticAsserts_ 1

#if UseStaticAsserts_
#define StaticAssert_(x) static_assert(x, #x);
#define StaticAssertMsg_(x, msg) static_assert(x, #x ", " msg);
#else
#define StaticAssert_(x)
    #define StaticAssertMsg_(x, msg)
#endif


#endif //APPLICATION_ASSERT_H
