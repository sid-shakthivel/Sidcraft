for (unsigned int i = 0; i < 10; i++)
{
    Chunk NewChunk = Chunk();
    NewChunk.CreateMesh();

    Matrix4f ModelMatrix = Matrix4f(1);
    ModelMatrix.Translate(Vector3f(i * 2, 0, 0));

    ChunkShader.Use();
    ChunkShader.SetMatrix4f("model", (const float *)(&ModelMatrix));
    ChunkShader.SetMatrix4f("view", (const float *)(&ViewMatrix));
    ChunkShader.SetMatrix4f("projection", (const float *)(&ProjectionMatrix));

    NewChunk.Draw(&ChunkShader);
}