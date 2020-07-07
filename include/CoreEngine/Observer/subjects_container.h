// ----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Author : Nicolas Schneider
// Date : 04/01/2020
// ----------------------------------------------------------------------------------
#pragma once

#include <optional_custom.h>

#include <CoreEngine/Observer/subject.h>
#include <CoreEngine/cassert.h>
#include <Chunks/chunks.h>

namespace poke::observer {

enum class SubjectType : uint8_t {
    NONE = 0,
    VOID,
    UNSIGNED_INT
};

using SubjectVoid = Subject<>;
using SubjectUnsignedInt = Subject<unsigned int>;

/**
 * \brief Contains anonymous subject
 */
template <typename T>
class SubjectsContainer {
public:
    SubjectsContainer() = default;

    explicit SubjectsContainer(
        const std::vector<T>& subjectsVoid = {},
        const std::vector<T>& subjectsUnsignedInt = {})
    {
        for (const T subject : subjectsVoid) {
            AddSubject(subject, SubjectType::VOID);
        }

        for (const T subject : subjectsUnsignedInt) {
            AddSubject(subject, SubjectType::UNSIGNED_INT);
        }
    }

    void AddSubject(T subjectEnumType, SubjectType type)
    {
        const int index = static_cast<int>(subjectEnumType);
        if (subjectTypes_.size() <= index) {
            subjectsVoid_.resize(index + 1);
            subjectsUnsignedInt_.resize(index + 1);

            subjectTypes_.resize(index + 1);
        }

        subjectTypes_[index] = type;

        switch (type) {
        case SubjectType::NONE:
            cassert(
                type != SubjectType::NONE,
                "Impossible to add a subject with a type == NONE.");
            break;
        case SubjectType::VOID:
            subjectsVoid_[index].emplace(SubjectVoid());
            break;
        case SubjectType::UNSIGNED_INT:
            subjectsUnsignedInt_[index].
                emplace(SubjectUnsignedInt());
            break;
        default:
            cassert(
                false,
                "This subject type " + std::to_string(static_cast<int>(
                    type)) + " is not handled.");
        }
    }

    /**
     * \brief Add an observer to a given subject
     * \param subjectTypeEnum 
     * \param observerCallback 
     */
    void AddObserver(
        T subjectTypeEnum,
        const std::function<void()>& observerCallback)
    {
        const int index = static_cast<int>(subjectTypeEnum);

        switch (subjectTypes_[index]) {
        case SubjectType::NONE:
            cassert(
                subjectTypes_[index] != SubjectType::NONE,
                "Impossible to add an observer, the subject container seems to no be holding any subject of the given type");
            break;
        case SubjectType::VOID:
            subjectsVoid_[index]->AddObserver(observerCallback);
            break;
        default:
            cassert(
                false,
                "The given callback std::function<void()> cannot be assigned to the subject");;
        }
    }

    void AddObserver(
        T subjectTypeEnum,
        const std::function<void(unsigned int)>& observerCallback)
    {
        const int index = static_cast<int>(subjectTypeEnum);

        switch (subjectTypes_[index]) {
        case SubjectType::NONE:
            cassert(
                subjectTypes_[index] != SubjectType::NONE,
                "Impossible to add an observer, the subject container seems to no be holding any subject of the given type");
            break;
        case SubjectType::UNSIGNED_INT:
            subjectsUnsignedInt_[index]->AddObserver(observerCallback);
            break;
        default:
            cassert(
                false,
                "The given callback std::function<void()> cannot be assigned to the subject");
        }
    }

	/**
	 * \brief Add an observer to a given subject
	 * \param subjectTypeEnum
	 * \param observerCallback
	 */
	void RemoveObserver(
		T subjectTypeEnum,
		const std::function<void()>& observerCallback)
	{
		const int index = static_cast<int>(subjectTypeEnum);

		switch (subjectTypes_[index]) {
		case SubjectType::NONE:
			cassert(
				subjectTypes_[index] != SubjectType::NONE,
				"Impossible to add an observer, the subject container seems to no be holding any subject of the given type");
			break;
		case SubjectType::VOID:
			subjectsVoid_[index]->RemoveObserver(observerCallback);
			break;
		default:
			cassert(
				false,
				"The given callback std::function<void()> cannot be assigned to the subject");;
		}
	}

	void RemoveObserver(
		T subjectTypeEnum,
		const std::function<void(unsigned int)>& observerCallback)
	{
		const int index = static_cast<int>(subjectTypeEnum);

		switch (subjectTypes_[index]) {
		case SubjectType::NONE:
			cassert(
				subjectTypes_[index] != SubjectType::NONE,
				"Impossible to add an observer, the subject container seems to no be holding any subject of the given type");
			break;
		case SubjectType::UNSIGNED_INT:
			subjectsUnsignedInt_[index]->RemoveObserver(observerCallback);
			break;
		default:
			cassert(
				false,
				"The given callback std::function<void()> cannot be assigned to the subject");
		}
	}

    /**
     * \brief Notify a subject
     * \param subjectEnumType
     */
    void NotifySubject(T subjectEnumType) const
    {
        const int index = static_cast<int>(subjectEnumType);

        switch (subjectTypes_[index]) {
        case SubjectType::NONE:
            cassert(
                subjectTypes_[index] != SubjectType::NONE,
                "Impossible to notify this subject, it's seems that this subject doesn't belong to this subject container.");
            break;
        case SubjectType::VOID:
            subjectsVoid_[index]->Notify();
            break;
        default: ;
        }
    }

    /**
     * \brief Notify a subject
     * \param subjectEnumType
     * \param value
     */
    void NotifySubject(
        T subjectEnumType,
        const unsigned int value) const
    {
        const int index = static_cast<int>(subjectEnumType);

        switch (subjectTypes_[index]) {
        case SubjectType::NONE:
            cassert(
                subjectTypes_[index] != SubjectType::NONE,
                "Impossible to notify this subject, it's seems that this subject doesn't belong to this subject container.");
            break;
        case SubjectType::UNSIGNED_INT:
            subjectsUnsignedInt_[index]->Notify(value);
            break;
        default: ;
        }
    }

    void Clear() {
        for (auto& subjectVoid : subjectsVoid_) {
			if (subjectVoid)
			{
				subjectVoid->Clear();
			}
        }
        for (auto& subjectEntityIndex : subjectsUnsignedInt_) {
			if (subjectEntityIndex)
			{
				subjectEntityIndex->Clear();
			}
        }
    }

private:
    std::vector<SubjectType> subjectTypes_;

    std::vector<std::experimental::optional<SubjectVoid>> subjectsVoid_;

    std::vector<std::experimental::optional<SubjectUnsignedInt>>
    subjectsUnsignedInt_;
};
} //namespace poke::observer
