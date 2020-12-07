// Copyright (c) 2012-2020 Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Platform/Platform.h"
#include "Engine/Core/Collections/Array.h"

/// <summary>
/// Tracking of per-resource or per-subresource state for GPU resources that require to issue resource access barriers during rendering.
/// </summary>
template<typename StateType, StateType InvalidState>
class GPUResourceState
{
private:

    /// <summary>
    /// The whole resource state (used only if _allSubresourcesSame is 1).
    /// </summary>
    StateType _resourceState : 31;

    /// <summary>
    /// Set to 1 if _resourceState is valid. In this case, all subresources have the same state.
    /// Set to 0 if _subresourceState is valid. In this case, each subresources may have a different state (or may be unknown).
    /// </summary>
    uint32 _allSubresourcesSame : 1;

    /// <summary>
    /// The per subresource state (used only if _allSubresourcesSame is 0).
    /// </summary>
    Array<StateType> _subresourceState;

public:

    /// <summary>
    /// Initializes a new instance of the <see cref="GPUResourceState"/> class.
    /// </summary>
    GPUResourceState()
        : _resourceState(InvalidState)
        , _allSubresourcesSame(true)
    {
    }

public:

    void Initialize(uint32 subresourceCount, StateType initialState, bool usePerSubresourceTracking)
    {
        ASSERT(_subresourceState.IsEmpty() && subresourceCount > 0);

        // Allocate space for per-subresource tracking structures
        // Note: for resources with single subresource we don't use this table
        if (usePerSubresourceTracking && subresourceCount > 1)
            _subresourceState.Resize(subresourceCount, false);

        // Initialize state
        _allSubresourcesSame = true;
        _resourceState = initialState;
#if BUILD_DEBUG
        _subresourceState.SetAll(InvalidState);
#endif
    }

    bool IsInitializated() const
    {
        return _resourceState != InvalidState || _subresourceState.HasItems();
    }

    void Release()
    {
        _resourceState = InvalidState;
        _subresourceState.Resize(0);
    }

    bool AreAllSubresourcesSame() const
    {
        return _allSubresourcesSame;
    }

    int32 GetSubresourcesCount() const
    {
        return _subresourceState.Count();
    }

    bool CheckResourceState(StateType state) const
    {
        if (_allSubresourcesSame)
        {
            return state == _resourceState;
        }

        // All subresources must be individually checked
        const uint32 numSubresourceStates = _subresourceState.Count();
        for (uint32 i = 0; i < numSubresourceStates; i++)
        {
            if (_subresourceState[i] != state)
            {
                return false;
            }
        }

        return true;
    }

    StateType GetSubresourceState(uint32 subresourceIndex) const
    {
        if (_allSubresourcesSame)
        {
            return _resourceState;
        }

        ASSERT(subresourceIndex >= 0 && subresourceIndex < static_cast<uint32>(_subresourceState.Count()));
        return _subresourceState[subresourceIndex];
    }

    void SetResourceState(StateType state)
    {
        _allSubresourcesSame = 1;
        _resourceState = state;

        // State is now tracked per-resource, so _subresourceState should not be read
#if BUILD_DEBUG
        const uint32 numSubresourceStates = _subresourceState.Count();
        for (uint32 i = 0; i < numSubresourceStates; i++)
        {
            _subresourceState[i] = InvalidState;
        }
#endif
    }

    void SetSubresourceState(int32 subresourceIndex, StateType state)
    {
        // If setting all subresources, or the resource only has a single subresource, set the per-resource state
        if (subresourceIndex == -1 || _subresourceState.Count() <= 1)
        {
            SetResourceState(state);
        }
        else
        {
            ASSERT(subresourceIndex < static_cast<int32>(_subresourceState.Count()));

            // If state was previously tracked on a per-resource level, then transition to per-subresource tracking
            if (_allSubresourcesSame)
            {
                const int32 numSubresourceStates = _subresourceState.Count();
                for (int32 i = 0; i < numSubresourceStates; i++)
                {
                    _subresourceState[i] = _resourceState;
                }

                _allSubresourcesSame = 0;

                // State is now tracked per-subresource, so _resourceState should not be read
#if BUILD_DEBUG
                _resourceState = InvalidState;
#endif
            }

            _subresourceState[subresourceIndex] = state;
        }
    }
};