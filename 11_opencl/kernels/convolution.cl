__kernel void multiply_complex(
    __global float2* a,
    __global float2* b,
    __global float2* result,
    const int numElements)
{
    int id = get_global_id(0);
    if (id < numElements) {
        float2 af = a[id];
        float2 bf = b[id];
        result[id] = (float2)(af.x * bf.x - af.y * bf.y, af.x * bf.y + af.y * bf.x);
    }
}
