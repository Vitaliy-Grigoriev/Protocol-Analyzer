// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <unordered_map>

#include "../include/analyzer/Log.hpp"


namespace analyzer::log
{
    /**
      * @typedef typedef std::unordered_map<int32_t, std::string> error_description;
      * @brief The type of the errors description.
      */
    using error_description = std::unordered_map<int32_t, std::string>;


    std::mutex log_mutex = { };
    /**
      * @var static error_description errors;
      * @brief Container that consist of the string definitions of system errors.
      */
    static error_description errors;

    /**
      * @fn static void SetErrorStrings(void) noexcept;
      * @brief Function that fills the container of the system error values.
      */
    static void SetErrorStrings(void) noexcept;

    StrSysError::StrSysError(void) noexcept { SetErrorStrings(); }

    StrSysError& StrSysError::Instance(void) noexcept
    {
        // Since it's a static variable, if the class has already been created, its won't be created again.
        // It's thread-safe in C++11.
        static StrSysError instance;
        return instance;
    }


    std::string StrSysError::operator() (const int32_t error) const noexcept
    {
        auto it = errors.find(error);
        if (it != errors.end()) { return (*it).second; }
        return std::string("Occurred unknown error (" + std::to_string(error) + ").");
    }

    static inline unsigned char charToUChar (char input)
    {
        union {
            char in;
            unsigned char out;
        } u { input };
        return u.out;
    }


    // TODO: Add three flag: is_offset, is_data, is_upper...
    // TODO: Add dependency length of the offset from the length of data.
    void DbgHexDump (const char* message, const void* data, std::size_t size, std::size_t hexLineLength)
    {
        if (hexLineLength % 2 == 1) { hexLineLength++; }
        if (hexLineLength < 8) { hexLineLength = 8; }
        const std::size_t full_chunks = size / hexLineLength;
        const std::size_t last_chunk = size % hexLineLength;
        const std::size_t mean_length = hexLineLength / 2;

        auto pSource = static_cast<const char*>(data);
        const std::size_t hex_lines = full_chunks + (last_chunk == 0 ? 0 : 1) + 2;
        const std::size_t hex_dump_line_length = 11 + 4 * hexLineLength + 8;
        const std::size_t hex_dump_size = hex_dump_line_length * hex_lines + 1;

        // Make hex dump header (2 lines).
        std::string hex_dump(hex_dump_size, ' ');
        hex_dump.replace(1, 8, "shift  |");
        for (std::size_t idx = 0; idx < hexLineLength; ++idx) {
            if (idx < mean_length) { hex_dump.replace(12 + idx * 3, 2, common::getHexValue(idx)); }
            else { hex_dump.replace(13 + idx * 3, 2, common::getHexValue(idx)); }
        }
        hex_dump.replace(hexLineLength * 3 + 17, 4, "data");
        hex_dump[hex_dump_line_length - 1] = '\n';
        hex_dump.replace(hex_dump_line_length, hex_dump_line_length - 1, hex_dump_line_length - 1, '-');
        hex_dump[hex_dump_line_length + 8] = '|';
        hex_dump[hex_dump_line_length * 2 - 1] = '\n';

        // Output hex data content.
        for (std::size_t idx = 0; idx < hex_lines - 2; ++idx)
        {
            const std::size_t line = hex_dump_line_length * (idx + 2);
            hex_dump.replace(line, 11, common::getHexValue(idx * hexLineLength, 8) + '|');
            for (std::size_t i = 0; i < hexLineLength && size != 0; ++i)
            {
                if (i < mean_length) {
                    hex_dump.replace(line + 12 + i * 3, 2, common::getHexValue(charToUChar(*pSource)));
                }
                else { hex_dump.replace(line + 13 + i * 3, 2, common::getHexValue(charToUChar(*pSource))); }
                pSource++;
                size--;
            }

            const std::size_t hex_data = hexLineLength * 3;
            pSource -= hexLineLength;
            size += hexLineLength;
            for (std::size_t i = 0; i < hexLineLength && size != 0; ++i)
            { // bug with cyrillic symbols...
                if (i < mean_length)
                {
                    if (isprint(static_cast<int32_t>(*pSource)) != 0) {
                        hex_dump.replace(line + hex_data + 17 + i, 1, pSource, 1);
                    } else {
                        hex_dump.replace(line + hex_data + 17 + i, 1, 1, '.');
                    }
                } else
                {
                    if (isprint(static_cast<int32_t>(*pSource)) != 0) {
                        hex_dump.replace(line + hex_data + 18 + i, 1, pSource, 1);
                    } else {
                        hex_dump.replace(line + hex_data + 18 + i, 1, 1, '.');
                    }
                };
                pSource++;
                size--;
            }
            hex_dump[line + hex_dump_line_length - 1] = '\n';
        }

        __common_log(LEVEL::TRACE, message, '\n', hex_dump, '\n');
    }


    // For Linux only.
    static void SetErrorStrings(void) noexcept
    {
        errors.emplace( std::make_pair(EPERM,           "'Operation not permitted'."));
        errors.emplace( std::make_pair(ENOENT,          "'No such file or directory'."));
        errors.emplace( std::make_pair(ESRCH,           "'No thread with the ID thread could be found'."));
        errors.emplace( std::make_pair(EINTR,           "'Interrupted system call'."));
        errors.emplace( std::make_pair(EIO,             "'I/O error'."));
        errors.emplace( std::make_pair(ENXIO,           "'No such device or address'."));
        errors.emplace( std::make_pair(E2BIG,           "'Arg list too long'."));
        errors.emplace( std::make_pair(ENOEXEC,         "'Exec format error'."));
        errors.emplace( std::make_pair(EBADF,           "'Bad file number'."));
        errors.emplace( std::make_pair(ECHILD,          "'No child processes'."));
        errors.emplace( std::make_pair(EAGAIN,          "'Try again'."));
        errors.emplace( std::make_pair(ENOMEM,          "'Out of memory'."));
        errors.emplace( std::make_pair(EACCES,          "'Permission denied'."));
        errors.emplace( std::make_pair(EFAULT,          "'Bad address'."));
        errors.emplace( std::make_pair(ENOTBLK,         "'Block device required'."));
        errors.emplace( std::make_pair(EBUSY,           "'Device or resource busy'."));
        errors.emplace( std::make_pair(EEXIST,          "'File exists'."));
        errors.emplace( std::make_pair(EXDEV,           "'Cross-device link'."));
        errors.emplace( std::make_pair(ENODEV,          "'No such device'."));
        errors.emplace( std::make_pair(ENOTDIR,         "'Not a directory'."));
        errors.emplace( std::make_pair(EISDIR,          "'Is a directory'."));
        errors.emplace( std::make_pair(EINVAL,          "'Invalid argument'."));
        errors.emplace( std::make_pair(ENFILE,          "'File table overflow'."));
        errors.emplace( std::make_pair(EMFILE,          "'Too many open files'."));
        errors.emplace( std::make_pair(ENOTTY,          "'Not a typewriter'."));
        errors.emplace( std::make_pair(ETXTBSY,         "'Text file busy'."));
        errors.emplace( std::make_pair(EFBIG,           "'File too large'."));
        errors.emplace( std::make_pair(ENOSPC,          "'No space left on device'."));
        errors.emplace( std::make_pair(ESPIPE,          "'Illegal seek'."));
        errors.emplace( std::make_pair(EROFS,           "'Read-only file system'."));
        errors.emplace( std::make_pair(EMLINK,          "'Too many links'."));
        errors.emplace( std::make_pair(EPIPE,           "'Broken pipe'."));
        errors.emplace( std::make_pair(EDEADLOCK,       "'Resource deadlock would occur'."));
        errors.emplace( std::make_pair(ENAMETOOLONG,    "'File name too long'."));
        errors.emplace( std::make_pair(ENOLCK,          "'No record locks available'."));
        errors.emplace( std::make_pair(ENOSYS,          "'Function not implemented'."));
        errors.emplace( std::make_pair(ENOTEMPTY,       "'Directory not empty'."));
        errors.emplace( std::make_pair(ELOOP,           "'Too many symbolic links encountered'."));
        errors.emplace( std::make_pair(EWOULDBLOCK,     "'Operation would block'."));
        errors.emplace( std::make_pair(ENOMSG,          "'No message of desired type'."));
        errors.emplace( std::make_pair(EIDRM,           "'Identifier removed'."));
        errors.emplace( std::make_pair(ECHRNG,          "'Channel number out of range'."));
        errors.emplace( std::make_pair(EL2NSYNC,        "'Level 2 not synchronized'."));
        errors.emplace( std::make_pair(EL3HLT,          "'Level 3 halted'."));
        errors.emplace( std::make_pair(EL3RST,          "'Level 3 reset'."));
        errors.emplace( std::make_pair(ELNRNG,          "'Link number out of range'."));
        errors.emplace( std::make_pair(EUNATCH,         "'Protocol driver not attached'."));
        errors.emplace( std::make_pair(ENOCSI,          "'No CSI structure available'."));
        errors.emplace( std::make_pair(EL2HLT,          "'Level 2 halted'."));
        errors.emplace( std::make_pair(EBADE,           "'Invalid exchange'."));
        errors.emplace( std::make_pair(EBADR,           "'Invalid request descriptor'."));
        errors.emplace( std::make_pair(EXFULL,          "'Exchange full'."));
        errors.emplace( std::make_pair(ENOANO,          "'No anode'."));
        errors.emplace( std::make_pair(EBADRQC,         "'Invalid request code'."));
        errors.emplace( std::make_pair(EBADSLT,         "'Invalid slot'."));
        errors.emplace( std::make_pair(EBFONT,          "'Bad font file format'."));
        errors.emplace( std::make_pair(ENOSTR,          "'Device not a stream'."));
        errors.emplace( std::make_pair(ENODATA,         "'No data available'."));
        errors.emplace( std::make_pair(ETIME,           "'Timer expired'."));
        errors.emplace( std::make_pair(ENOSR,           "'Out of streams resources'."));
        errors.emplace( std::make_pair(ENONET,          "'Machine is not on the network'."));
        errors.emplace( std::make_pair(ENOPKG,          "'Package not installed'."));
        errors.emplace( std::make_pair(EREMOTE,         "'Object is remote'."));
        errors.emplace( std::make_pair(ENOLINK,         "'Link has been severed'."));
        errors.emplace( std::make_pair(EADV,            "'Advertise error'."));
        errors.emplace( std::make_pair(ESRMNT,          "'Srmount error'."));
        errors.emplace( std::make_pair(ECOMM ,          "'Communication error on send'."));
        errors.emplace( std::make_pair(EPROTO,          "'Protocol error'."));
        errors.emplace( std::make_pair(EMULTIHOP,       "'Multihop attempted'."));
        errors.emplace( std::make_pair(EDOTDOT,         "'RFS specific error'."));
        errors.emplace( std::make_pair(EBADMSG,         "'Not a data message'."));
        errors.emplace( std::make_pair(EOVERFLOW,       "'Value too large for defined data type'."));
        errors.emplace( std::make_pair(ENOTUNIQ,        "'Name not unique on network'."));
        errors.emplace( std::make_pair(EBADFD,          "'File descriptor in bad state'."));
        errors.emplace( std::make_pair(EREMCHG,         "'Remote address changed'."));
        errors.emplace( std::make_pair(ELIBACC,         "'Can not access a needed shared library'."));
        errors.emplace( std::make_pair(ELIBBAD,         "'Accessing a corrupted shared library'."));
        errors.emplace( std::make_pair(ELIBSCN,         "'.lib section in a.out corrupted'."));
        errors.emplace( std::make_pair(ELIBMAX,         "'Attempting to link in too many shared libraries'."));
        errors.emplace( std::make_pair(ELIBEXEC,        "'Cannot exec a shared library directly'."));
        errors.emplace( std::make_pair(ERESTART,        "'Interrupted system call should be restarted'."));
        errors.emplace( std::make_pair(ESTRPIPE,        "'Streams pipe error'."));
        errors.emplace( std::make_pair(EUSERS,          "'Too many users'."));
        errors.emplace( std::make_pair(ENOTSOCK,        "'Socket operation on non-socket'."));
        errors.emplace( std::make_pair(EDESTADDRREQ,    "'Destination address required'."));
        errors.emplace( std::make_pair(EMSGSIZE,        "'Message too long'."));
        errors.emplace( std::make_pair(EPROTOTYPE,      "'Protocol wrong type for socket'."));
        errors.emplace( std::make_pair(ENOPROTOOPT,     "'Protocol not available'."));
        errors.emplace( std::make_pair(EPROTONOSUPPORT, "'Protocol not supported'."));
        errors.emplace( std::make_pair(ESOCKTNOSUPPORT, "'Socket type not supported'."));
        errors.emplace( std::make_pair(EOPNOTSUPP,      "'Operation not supported on transport endpoint'."));
        errors.emplace( std::make_pair(EPFNOSUPPORT,    "'Protocol family not supported'."));
        errors.emplace( std::make_pair(EAFNOSUPPORT,    "'Address family not supported by protocol'."));
        errors.emplace( std::make_pair(EADDRINUSE,      "'Address already in use'."));
        errors.emplace( std::make_pair(EADDRNOTAVAIL,   "'Cannot assign requested address'."));
        errors.emplace( std::make_pair(ENETDOWN,        "'Network is down'."));
        errors.emplace( std::make_pair(ENETUNREACH,     "'Network is unreachable'."));
        errors.emplace( std::make_pair(ENETRESET,       "'Network dropped connection because of reset'."));
        errors.emplace( std::make_pair(ECONNABORTED,    "'Software caused connection abort'."));
        errors.emplace( std::make_pair(ECONNRESET,      "'Connection reset by peer'."));
        errors.emplace( std::make_pair(ENOBUFS,         "'No buffer space available'."));
        errors.emplace( std::make_pair(EISCONN,         "'Transport endpoint is already connected'."));
        errors.emplace( std::make_pair(ENOTCONN,        "'Transport endpoint is not connected'."));
        errors.emplace( std::make_pair(ESHUTDOWN,       "'Cannot send after transport endpoint shutdown'."));
        errors.emplace( std::make_pair(ETOOMANYREFS,    "'Too many references: cannot splice'."));
        errors.emplace( std::make_pair(ETIMEDOUT,       "'Connection timed out'."));
        errors.emplace( std::make_pair(ECONNREFUSED,    "'Connection refused'."));
        errors.emplace( std::make_pair(EHOSTDOWN,       "'Host is down'."));
        errors.emplace( std::make_pair(EHOSTUNREACH,    "'No route to host'."));
        errors.emplace( std::make_pair(EALREADY,        "'Operation already in progress'."));
        errors.emplace( std::make_pair(EINPROGRESS,     "'Operation now in progress'."));
        errors.emplace( std::make_pair(ESTALE,          "'Stale NFS file handle'."));
        errors.emplace( std::make_pair(EUCLEAN,         "'Structure needs cleaning'."));
        errors.emplace( std::make_pair(ENOTNAM,         "'Not a XENIX named type file'."));
        errors.emplace( std::make_pair(ENAVAIL,         "'No XENIX semaphores available'."));
        errors.emplace( std::make_pair(EISNAM,          "'Is a named type file'."));
        errors.emplace( std::make_pair(EREMOTEIO,       "'Remote I/O error'."));
        errors.emplace( std::make_pair(EDQUOT,          "'Quota exceeded'."));
        errors.emplace( std::make_pair(ENOMEDIUM,       "'No medium found'."));
        errors.emplace( std::make_pair(EMEDIUMTYPE,     "'Wrong medium type'."));
        errors.emplace( std::make_pair(ECANCELED,       "'Operation canceled'."));
        errors.emplace( std::make_pair(ENOKEY,          "'Required key not available'."));
        errors.emplace( std::make_pair(EKEYEXPIRED,     "'Key has expired'."));
        errors.emplace( std::make_pair(EKEYREVOKED,     "'Key has been revoked'."));
        errors.emplace( std::make_pair(EKEYREJECTED,    "'Key was rejected by service'."));
        errors.emplace( std::make_pair(EOWNERDEAD,      "'Owner died'."));
        errors.emplace( std::make_pair(ENOTRECOVERABLE, "'State not recoverable'."));
        errors.emplace( std::make_pair(ERFKILL,         "'Operation not possible due to RF-kill'."));
        errors.emplace( std::make_pair(EHWPOISON,       "'Memory page has hardware error'."));
    }

}  // namespace log.
