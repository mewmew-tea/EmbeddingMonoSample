#include <iostream>
#include <cstdio>
#include <cstdint>
// Mono
#pragma comment (lib, "mono-2.0-sgen.lib")
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>

// C#側から呼び出される関数
int32_t Multiply(int32_t a, int32_t b)
{
	return a * b;
}

int main()
{
	// Monoのアセンブリと設定ファイルのディレクトリをセットする
	mono_set_dirs("./MonoAssembly/bin/", "./MonoAssembly/etc/");
	
	// ドメイン（OSにおけるプロセスのようなもの）
	MonoDomain* domain = nullptr;
	// Monoの初期化
	domain = mono_jit_init("CSScriptTest");
	if (!domain)
	{
		printf("Monoの初期化に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// スクリプトのアセンブリ（中間言語の状態に変換したC#）
	MonoAssembly* assembly = nullptr;
	// スクリプトのアセンブリ(DLL)をロード
	assembly = mono_domain_assembly_open(domain, ".\\CSScript.dll");
	if (!assembly)
	{
		printf("スクリプトのアセンブリのロードに失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}
	// アセンブリのイメージ（アセンブリ内のコード情報を実際に保持しているもの）
	MonoImage* assemblyImage = nullptr;
	assemblyImage = mono_assembly_get_image(assembly);
	if (!assemblyImage)
	{
		printf("スクリプトのアセンブリイメージの取得に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// クラスの型
	MonoClass* mainClass = nullptr;
	mainClass = mono_class_from_name(assemblyImage, "CSScript", "Class1");
	if (!mainClass)
	{
		printf("クラスの型取得に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// クラスのインスタンスを作成
	MonoObject* classInstance = nullptr;
	classInstance = mono_object_new(domain, mainClass);
	if (!classInstance)
	{
		printf("クラスのインスタンス生成に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// 関数情報定義
	MonoMethodDesc* methodDesc = nullptr;
	methodDesc = mono_method_desc_new("CSScript.Class1::PrintMessage()", true);
	if (!methodDesc)
	{
		printf("関数情報の定義作成に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// スクリプトの関数
	MonoMethod* method = nullptr;
	// 関数情報定義をもとに、クラス内の関数を検索
	method = mono_method_desc_search_in_class(methodDesc, mainClass);
	if (!method)
	{
		printf("関数取得に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// 関数実行時の例外情報
	MonoObject* excObject = nullptr;
	// 関数を呼び出し
	mono_runtime_invoke(method, classInstance, nullptr, &excObject);
	if (excObject)
	{
		MonoString* excString = mono_object_to_string(excObject, nullptr);
		const char* excCString = mono_string_to_utf8(excString);
		printf("関数実行時例外%s：\n", excCString);
		mono_jit_cleanup(domain);
		return 1;
	}

	//--------------------------------
	// C#からC++の関数を呼び出す
	//--------------------------------
	
	// C++の関数を、内部呼び出し対象として登録
	mono_add_internal_call("CSScript.Class1::Multiply", &Multiply);

	// 関数情報定義２
	MonoMethodDesc* methodDesc2 = nullptr;
	methodDesc2 = mono_method_desc_new("CSScript.Class1::PrintMessage2()", true);
	if (!methodDesc2)
	{
		printf("関数情報の定義作成に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// スクリプトの関数２
	MonoMethod* method2 = nullptr;
	// 関数情報定義をもとに、クラス内の関数を検索
	method2 = mono_method_desc_search_in_class(methodDesc2, mainClass);
	if (!method2)
	{
		printf("関数取得に失敗\n");
		mono_jit_cleanup(domain);
		return 1;
	}

	// 関数実行時の例外情報２
	MonoObject* excObject2 = nullptr;
	// 関数を呼び出し２
	mono_runtime_invoke(method2, classInstance, nullptr, &excObject2);
	if (excObject2)
	{
		MonoString* excString = mono_object_to_string(excObject2, nullptr);
		const char* excCString = mono_string_to_utf8(excString);
		printf("関数実行時例外%s：\n", excCString);
		mono_jit_cleanup(domain);
		return 1;
	}


	// Monoの終了処理
	mono_jit_cleanup(domain);

	return 0;
}
