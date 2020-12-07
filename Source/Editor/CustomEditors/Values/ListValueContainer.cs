// Copyright (c) 2012-2020 Wojciech Figat. All rights reserved.

using System;
using System.Collections;
using FlaxEditor.Scripting;

namespace FlaxEditor.CustomEditors
{
    /// <summary>
    /// Custom <see cref="ValueContainer"/> for <see cref="IList"/> (used for <see cref="Array"/> and <see cref="System.Collections.Generic.List{T}"/>.
    /// </summary>
    /// <seealso cref="FlaxEditor.CustomEditors.ValueContainer" />
    public class ListValueContainer : ValueContainer
    {
        /// <summary>
        /// The index in the collection.
        /// </summary>
        public readonly int Index;

        /// <summary>
        /// Initializes a new instance of the <see cref="ListValueContainer"/> class.
        /// </summary>
        /// <param name="elementType">Type of the collection elements.</param>
        /// <param name="index">The index.</param>
        public ListValueContainer(ScriptType elementType, int index)
        : base(ScriptMemberInfo.Null, elementType)
        {
            Index = index;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ListValueContainer"/> class.
        /// </summary>
        /// <param name="elementType">Type of the collection elements.</param>
        /// <param name="index">The index.</param>
        /// <param name="values">The collection values.</param>
        public ListValueContainer(ScriptType elementType, int index, ValueContainer values)
        : this(elementType, index)
        {
            Capacity = values.Count;
            for (int i = 0; i < values.Count; i++)
            {
                var v = (IList)values[i];
                Add(v[index]);
            }

            if (values.HasReferenceValue)
            {
                var v = (IList)values.ReferenceValue;

                // Get the reference value if collections are the same size
                if (v != null && values.Count == v.Count)
                {
                    _referenceValue = v[index];
                    _hasReferenceValue = true;
                }
            }
        }

        /// <inheritdoc />
        public override void Refresh(ValueContainer instanceValues)
        {
            if (instanceValues == null || instanceValues.Count != Count)
                throw new ArgumentException();

            for (int i = 0; i < Count; i++)
            {
                var v = (IList)instanceValues[i];
                this[i] = v[Index];
            }
        }

        /// <inheritdoc />
        public override void Set(ValueContainer instanceValues, object value)
        {
            if (instanceValues == null || instanceValues.Count != Count)
                throw new ArgumentException();

            for (int i = 0; i < Count; i++)
            {
                var v = (IList)instanceValues[i];
                v[Index] = value;
                this[i] = value;
            }
        }

        /// <inheritdoc />
        public override void Set(ValueContainer instanceValues, ValueContainer values)
        {
            if (instanceValues == null || instanceValues.Count != Count)
                throw new ArgumentException();
            if (values == null || values.Count != Count)
                throw new ArgumentException();

            for (int i = 0; i < Count; i++)
            {
                var v = (IList)instanceValues[i];
                var value = ((ListValueContainer)values)[i];
                v[Index] = value;
                this[i] = value;
            }
        }

        /// <inheritdoc />
        public override void Set(ValueContainer instanceValues)
        {
            if (instanceValues == null || instanceValues.Count != Count)
                throw new ArgumentException();

            for (int i = 0; i < Count; i++)
            {
                var v = (IList)instanceValues[i];
                v[Index] = this[i];
            }
        }

        /// <inheritdoc />
        public override void RefreshReferenceValue(object instanceValue)
        {
            if (instanceValue is IList v)
            {
                _referenceValue = v[Index];
                _hasReferenceValue = true;
            }
        }
    }
}