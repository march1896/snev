#ifndef _SHADER_H_
#define _SHADER_H_
class Shader {
public:
	Shader();
	Shader( const char* VsFilename, const char *FsFilename );
	~Shader();
	void SetVsFile( const char* filename );
	void SetFsFile( const char* filename );
	void Activate();
	void Deactivate();

	bool SetUniform( const char *variableName, float p1 );
	bool SetUniform( const char *varialbeName, float p1, float p2 );
	bool SetUniform( const char *variableName, float p1, float p2, float p3 );
	bool SetUniform( const char *variableName, float p1, float p2, float p3, float p4 );

	bool SetAttribute( const char *vn, float p1 );
	bool SetAttribute( const char *vn, float p1, float p2 );
	bool SetAttribute( const char *vn, float p1, float p2, float p3 );
	bool SetAttribute( const char *vn, float p1, float p2, float p3, float p4 );

	void PrintShaderInfo();
	void PrintProgramInfo();
	static bool IsSupported();
private:
	char *vsFile, *fsFile;
	char *vsSource, *fsSource;
	int vs, fs;
	int program;
	bool inited;
};

#endif
