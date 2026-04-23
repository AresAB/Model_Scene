#pragma once

unsigned int check_gl_errors(int line) {
    unsigned int ret = 0;
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) ret = 1;
    while(error != GL_NO_ERROR) {
        switch(error) {
            case GL_INVALID_ENUM:
                printf("|%s|\n(%d) ERROR: GL_INVALID_ENUM %u\n", __FILE__, line, error);
                printf("- An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
                break;
            case GL_INVALID_VALUE:
                printf("|%s|\n(%d) ERROR: GL_INVALID_VALUE %u\n", __FILE__, line, error);
                printf("- A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
                break;
            case GL_INVALID_OPERATION:
                printf("|%s|\n(%d) ERROR: GL_INVALID_OPERATION %u\n", __FILE__, line, error);
                printf("- The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("|%s|\n(%d) ERROR: GL_INVALID_FRAMEBUFFER_OPERATION %u\n", __FILE__, line, error);
                printf("-  The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.");
                break;
            case GL_OUT_OF_MEMORY:
                printf("|%s|\n(%d) ERROR: GL_OUT_OF_MEMORY %u\n", __FILE__, line, error);
                printf("- There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
                break;
            default:
                printf("|%s|\n(%d) BAD ERROR %u", __FILE__, line, error);
                break;
        }
        error = glGetError();
    }
    return ret;
}