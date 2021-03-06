// Module:  Log4CPLUS
// File:    appenderattachableimpl.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2009 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/helpers/apndimpl.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/spi/logevent.h"

//#include <algorithm>


namespace log4cplus
{


namespace spi
{


AppenderAttachable::~AppenderAttachable()
{ }


} // namespace spi


namespace helpers
{


//////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl ctor and dtor
//////////////////////////////////////////////////////////////////////////////

AppenderAttachableImpl::AppenderAttachableImpl()
 : appender_list_mutex(LOG4CPLUS_MUTEX_CREATE)
{
}


AppenderAttachableImpl::~AppenderAttachableImpl()
{
   LOG4CPLUS_MUTEX_FREE( appender_list_mutex );
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl public methods
///////////////////////////////////////////////////////////////////////////////

void
AppenderAttachableImpl::addAppender(SharedAppenderPtr newAppender)
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        if(newAppender == NULL) {
            getLogLog().warn( LOG4CPLUS_TEXT("Tried to add NULL appender") );
            return;
        }

        ListIteratorType it = appenderList.begin();
        while (it != appenderList.end())
        {
            if (*it == newAppender)
                break;
            it++;
        }

        if(it == appenderList.end()) {
            appenderList.push_back(newAppender);
        }
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



AppenderAttachableImpl::ListType
AppenderAttachableImpl::getAllAppenders()
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        return appenderList;
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



SharedAppenderPtr
AppenderAttachableImpl::getAppender(const log4cplus::tstring& name)
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        for(ListIteratorType it=appenderList.begin();
            it!=appenderList.end();
            ++it)
        {
            if((*it)->getName() == name) {
                return *it;
            }
        }

        return SharedAppenderPtr(NULL);
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



void
AppenderAttachableImpl::removeAllAppenders()
{
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        appenderList.erase(appenderList.begin(), appenderList.end());
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



void
AppenderAttachableImpl::removeAppender(SharedAppenderPtr appender)
{
    if(appender == NULL) {
        getLogLog().warn( LOG4CPLUS_TEXT("Tried to remove NULL appender") );
        return;
    }

    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        ListIteratorType it = appenderList.begin();

        while (it != appenderList.end())
        {
            if (*it == appender)
                break;
            it++;
        }

        if(it != appenderList.end()) {
            appenderList.erase(it);
        }
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}



void
AppenderAttachableImpl::removeAppender(const log4cplus::tstring& name)
{
    removeAppender(getAppender(name));
}



int
AppenderAttachableImpl::appendLoopOnAppenders(const spi::InternalLoggingEvent& event) const
{
    int count = 0;

    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( appender_list_mutex )
        for(ListConstIteratorType it=appenderList.begin();
            it!=appenderList.end();
            ++it)
        {
            ++count;
            (*it)->doAppend(event);
        }
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;

    return count;
}


} // namespace helpers


} // namespace log4cplus
