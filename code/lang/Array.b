class Array<T> {
    public native T[] subarray(Int startIndex);
    public native T[] subarray(Int startIndex, Int endIndex);
    public native T[] copy();
    public native void copyTo(T[] buffer);
    public native Int length();
    
    public String join(String separator) {
        StringBuffer buffer = new StringBuffer();
        Bool first = true;
        for (T element : this) {
            if (!first)
                buffer.append(separator);
            first = false;
            buffer.append(element);
        }
        return buffer.toString();
    }
    
    public T[] resizedCopy(Int length) {
        T[] copy = new T[length];
        if (length < length())
            subarray(0, length).copyTo(copy);
        else
            copyTo(copy);
        return copy;
    }
    
    public T[] reverse() {
        T[] reverse = new T[length()];
        for (Int i = 0; i < length(); i++)
            reverse[i] = this[length() - i - 1];
        return reverse;
    }
    
    public <T2> T2[] map(function<T2, T> func) {
        T2[] mappedArray = new T2[length()];
        for (Int i = 0; i < length(); i++)
            mappedArray[i] = func(this[i]);
        return mappedArray;
    }
    
    public T[] filter(function<Bool, T> func) {
        T[] filteredArray = new T[length()];
        Int index = 0;
        for (T element : this) {
            if (func(element)) {
                filteredArray[index] = element;
                index++;
            }
        }
        // Call copy() in order to save on memory
        return filteredArray.subarray(0, index).copy();
    }
}
